import JavaScriptCore

func fetch(resource: JSValue, options: JSValue) -> JSValue {
    let session = URLSession.shared

    var url: URL?

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
                let jsErr = JSError(message: err.localizedDescription)
                reject?.call(withArguments: [jsErr as Any])
                return
            }

            guard let httpResponse = response as? HTTPURLResponse else {
                print("HTTPURLResponse was nil")
                let jsErr = JSError(message: "native response was nil")
                reject?.call(withArguments: [jsErr as Any])
                return
            }

            let res = JSResponse(from: httpResponse, responseData: data)
            resolve?.call(withArguments: [res])
            return
        }

        task.resume()
    }
}
