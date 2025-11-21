import JavaScriptCore

@objc protocol JSResponseExports: JSExport {
    // swiftlint:disable:next identifier_name
    var ok: Bool { get }
    var status: Int { get }
    var url: String { get }

    func text() -> JSValue
    func json() -> JSValue
}

@objc public class JSResponse: NSObject, JSResponseExports {
    dynamic var url: String
    dynamic var status: Int
    // swiftlint:disable:next identifier_name
    dynamic var ok: Bool {
        return status >= 200 && status <= 299
    }

    private let data: Data?

    public init(status: Int, responseData: Data?) {
        self.status = status
        self.data = responseData
        self.url = ""
    }

    public init(from httpResponse: HTTPURLResponse, responseData: Data?) {
        self.data = responseData
        self.status = httpResponse.statusCode
        self.url = httpResponse.url?.absoluteString ?? ""
    }

    func text() -> JSValue {
        return JSValue(newPromiseIn: JSContext.current()) { resolve, _ in
            guard let data = self.data else {
                resolve?.call(withArguments: [""])
                return
            }

            if let str = String(bytes: data, encoding: .utf8) {
                resolve?.call(withArguments: [str])
                return
            }

            resolve?.call(withArguments: [""])
        }
    }

    func json() -> JSValue {
        return JSValue(newPromiseIn: JSContext.current()) { resolve, reject in
            do {
                guard let data = self.data else {
                    resolve?.call(withArguments: [JSValue(nullIn: JSContext.current()) as Any])
                    return
                }

                let anyObj = try JSONSerialization.jsonObject(with: data)
                let jsonObj = JSValue(object: anyObj, in: JSContext.current())
                resolve?.call(withArguments: [jsonObj as Any])

            } catch {
                let err = JSError(message: error.localizedDescription)
                reject?.call(withArguments: [err as Any])
            }
        }
    }
}
