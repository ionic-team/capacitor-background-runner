import JavaScriptCore

func fetch(resource: JSValue, options: JSValue) -> JSValue {
    print("fetch called")
    
    let session = URLSession.shared
    
    var url: URL? = nil
    
    if resource.isString {
        url = URL(string: resource.toString())
    }
    
    guard let url = url else {
        return JSValue.init(newErrorFromMessage: "invalid url", in: JSContext.current())
    }
    
    var request = URLRequest(url: url)
    
    if options.isObject, let optionsDict = options.toDictionary() as? [String: Any] {
        request.httpMethod = (optionsDict["method"] as? String)?.uppercased()
        request.allHTTPHeaderFields = optionsDict["headers"] as? [String: String]
        
        if let bodyString = optionsDict["body"] as? String {
            request.httpBody = bodyString.data(using: .utf8)
        }
    }
    
    return JSValue(newPromiseIn: JSContext.current()) { resolve, reject in
        
        let task = session.dataTask(with: request) { data, response, err in
            if let err = err {
                print("fetch failed: \(err.localizedDescription)")                
                let jsErr = JSValue(newErrorFromMessage: "failed", in: JSContext.current())
                reject?.call(withArguments: ["\(err.localizedDescription)"])
                return
            }
            
            if let httpResponse = response as? HTTPURLResponse {
                print("fetch succeeded")
                let res = JSResponse(from: httpResponse, responseData: data)
                resolve?.call(withArguments: [res])
                return
            }
            
            print("????")
            resolve?.call(withArguments: [])
        }
        
        task.resume()
    }
}
