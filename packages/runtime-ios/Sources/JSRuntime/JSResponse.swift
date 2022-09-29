import Foundation
import JavaScriptCore

@objc protocol JSResponseExports: JSExport {
    var url: String { get }
    var status: Int { get }
    var statusText: String { get }
    var ok: Bool { get }
    
    func json() -> JSValue
    func text() -> JSValue
}

@objc public class JSResponse: NSObject, JSResponseExports {
    dynamic var url: String
    dynamic var status: Int
    dynamic var statusText: String
    dynamic var ok: Bool
    
    let data: Data?
    
    required init(data: Data?, response: HTTPURLResponse) {
        self.data = data
        
        if let url = response.url {
            self.url = url.absoluteString
        } else {
            self.url = ""
        }
        
        self.status = response.statusCode
        
        if response.statusCode >= 200 && response.statusCode <= 299 {
            self.statusText = "OK"
            self.ok = true
        } else {
            self.statusText = "Err"
            self.ok = false
        }
    }
    
    func json() -> JSValue {
        return JSValue(newPromiseIn: JSContext.current()) { resolve, reject in
            if let resolve = resolve, let data = self.data {
                do {
                    if let json = try JSONSerialization.jsonObject(with: data) as? [AnyHashable: Any] {
                        let jsJsonObj = JSValue(object: json, in: JSContext.current())
                        resolve.call(withArguments: [jsJsonObj])
                    } else {
                        if let reject = reject {
                            reject.call(withArguments: ["could not parse json object"])
                        }
                    }
                } catch {
                    if let reject = reject {
                        reject.call(withArguments: [error.localizedDescription])
                    }
                }
            }
        }
    }
    
    func text() -> JSValue {
        return JSValue(newPromiseIn: JSContext.current()) { resolve, reject in
            if let resolve = resolve {
                if let data = self.data, let text = String(data: data, encoding: .utf8) {
                    resolve.call(withArguments: [text])
                } else {
                    resolve.call(withArguments: [])
                }
            }
        }
    }
}
