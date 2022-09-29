import XCTest
@testable import JSRuntime

final class RuntimeTests: XCTestCase {
    func testContexts() throws {
        let contextId = "io.ionic.testContexts"
        let runner = Runner()
        _ = try runner.createContext(name: contextId)
        _ = runner.execute(name: contextId, code: "let test = 123;")
        guard let val = runner.execute(name: contextId, code: "test") else {
            XCTFail("js value was nil")
            return
        }
        
        XCTAssertTrue(val.isNumber)
        XCTAssertEqual(123, val.toInt32())
        
        _ = runner.execute(name: contextId, code: "test += 123;")
        
        guard let val = runner.execute(name: contextId, code: "test") else {
            XCTFail("js value was nil")
            return
        }
            
        XCTAssertTrue(val.isNumber)
        XCTAssertEqual(246, val.toInt32())
        
        runner.destroyContext(name: contextId)
        _ = try runner.createContext(name: contextId)
        guard let val = runner.execute(name: contextId, code: "test") else {
            XCTFail("js value was nil")
            return
        }
        
        XCTAssertTrue(val.isUndefined)
    }
    
    func testCustomEventObject() throws {
        let contextId = "io.ionic.testCustomEventObject"
        let runner = Runner()
        _ = try runner.createContext(name: contextId)
        _ = runner.execute(name: contextId, code: "const event = new CustomEvent('myEvent');")
        guard let customEventObj = runner.execute(name: contextId, code: "event") else {
            XCTFail("js value was nil")
            return
        }
        
        XCTAssertTrue(customEventObj.isObject)
        
        guard let type = customEventObj.objectForKeyedSubscript("type") else {
            XCTFail("js value was nil")
            return
        }
        
        XCTAssertTrue(type.isString)
        XCTAssertEqual("myEvent", type.toString())
        
        _ = runner.execute(name: contextId, code: "const eventWithDetails = new CustomEvent('myEventDetails', {detail: { name: 'cat' } });")
        guard let customEventObj = runner.execute(name: contextId, code: "eventWithDetails") else {
            XCTFail("js value was nil")
            return
        }
        
        XCTAssertTrue(customEventObj.isObject)
        
        _ = runner.execute(name: contextId, code: "console.log(eventWithDetails.detail.name);")
        
        guard let eventDetail = customEventObj.objectForKeyedSubscript("detail") else  {
            XCTFail("js value was nil")
            return
        }
        
        XCTAssertTrue(eventDetail.isObject)
        
        guard let eventDetailName = eventDetail.objectForKeyedSubscript("name") else {
            XCTFail("js value was nil")
            return
        }
        
        XCTAssertTrue(eventDetailName.isString)
        XCTAssertEqual("cat", eventDetailName.toString())
    }
    
    func testEventListeners() throws {
        let expectation = XCTestExpectation(description: "Run callback set on event listener")
        
        let testCallback: @convention(block) () -> Void = {
            expectation.fulfill()
        }
        
        let contextId = "io.ionic.testEventListeners"
        let runner = Runner()
        let context = try runner.createContext(name: contextId)
        context.setGlobalObject(obj: testCallback, forName: "eventCallback")
        _ = runner.execute(name: contextId, code: "addEventListener('push',  eventCallback);")
        
        
        Timer.scheduledTimer(withTimeInterval: 5, repeats: false) { _ in
            context.dispatchEvent(event: "push")
        }
        
        wait(for: [expectation], timeout: 10.0)
        
    }
    
    func testTextDecoder() throws {
        let contextId = "io.ionic.testTextEncoder"
        let runner = Runner()
        _ = try runner.createContext(name: contextId)
        
        _ = runner.execute(name: contextId, code: "const decoder = new TextDecoder();")
        
        guard let decoderObj = runner.execute(name: contextId, code: "decoder") else {
            XCTFail("decoder js object was nil")
            return
        }
        
        XCTAssertTrue(decoderObj.isObject)
        
        guard let strExample1 = runner.execute(name: contextId, code: "decoder.decode([226, 130, 172]);") else {
            XCTFail("example string 1 was nil")
            return
        }
        
        XCTAssertEqual(strExample1.toString(), "€")
        
        guard let strExample2 = runner.execute(name: contextId, code: "decoder.decode(new Int8Array([-16, -96, -82, -73]));") else {
            XCTFail("example string 2 was nil")
            return
        }
        
        XCTAssertEqual(strExample2.toString(), "𠮷")
        
        _ = runner.execute(name: contextId, code: "const winDecoder = new TextDecoder('windows-1251');")
        
        guard let strExample5 = runner.execute(name: contextId, code: "winDecoder.decode(new Uint8Array([207, 240, 232, 226, 229, 242, 44, 32, 236, 232, 240, 33]));") else {
            XCTFail("example string 5 was nil")
            return
        }
        
        XCTAssertEqual(strExample5.toString(), "Привет, мир!")
        
    }
    
    func testTextEncoder() throws {
        let contextId = "io.ionic.testTextEncoder"
        let runner = Runner()
        _ = try runner.createContext(name: contextId)
        
        _ = runner.execute(name: contextId, code: "let testString = \"Hello, world!\"; const encoder = new TextEncoder();")
        guard let encoderObj = runner.execute(name: contextId, code: "encoder") else {
            XCTFail("encoder js object was nil")
            return
        }
        
        XCTAssertTrue(encoderObj.isObject)
        
        guard let uintArr = runner.execute(name: contextId, code: "encoder.encode(testString);") else {
            XCTFail("encoded array object was nil")
            return
        }
        
        XCTAssertTrue(uintArr.isArray)
        
        guard let arr = uintArr.toArray() as? [UInt8] else {
            XCTFail("array is not uint8 array")
            return
        }
        
        XCTAssertEqual(arr.count, 13)
        
        let code = """
            let newArr = new Uint8Array(testString.length);
            let result = encoder.encodeInto(testString, newArr);
        """

        _ = runner.execute(name: contextId, code: code)
        
        guard let newUintArr = runner.execute(name: contextId, code: "newArr") else {
            XCTFail("new encoded array object was nil")
            return
        }
        
        guard let newArr = newUintArr.toArray() as? [UInt8] else {
            XCTFail("array is not uint8 array")
            return
        }
        
        XCTAssertEqual(newArr.count, 13)
        XCTAssertEqual(arr, newArr)
        
        guard let resultObj = runner.execute(name: contextId, code: "result") else {
            XCTFail("encodeInto result is nil")
            return
        }
        
        XCTAssertEqual(resultObj.objectForKeyedSubscript("read").toInt32(), 13)
        XCTAssertEqual(resultObj.objectForKeyedSubscript("written").toInt32(), 13)

    }
    
    func testPromise() throws {
        let code = """
            const myPromise = new Promise((res, reject) => {
                setTimeout(() => {
                    res("foo");
                  }, 200);
            });
        
            myPromise.then((res) => {
                console.log(res);
            })
        """
        let contextId = "io.ionic.testPromises"
        let runner = Runner()
        _ = try runner.createContext(name: contextId)
        _ = runner.execute(name: contextId, code: code)
        
    }
    
    func testCryptoRandomUUID() throws {
        let contextId = "io.ionic.testCryptoRandomUUID"
        let runner = Runner()
    
        _ = try runner.createContext(name: contextId)
        guard let generatedUUID = runner.execute(name: contextId, code: "crypto.randomUUID();") else {
            XCTFail("generatedUUID result is nil")
            return
        }
        
        let uuid = generatedUUID.toString()
        XCTAssertEqual(uuid?.count, 36)
    }
    
    func testCryptoGetRandomValues() throws {
        let contextId = "io.ionic.testCryptoGetRandomValues"
        let runner = Runner()
    
        _ = try runner.createContext(name: contextId)
        _ = runner.execute(name: contextId, code: "let array = new Uint8Array(10); crypto.getRandomValues(array);")
        
        guard let randomArrObj = runner.execute(name: contextId, code: "array") else {
            XCTFail("array result is nil")
            return
        }
        
        guard let uInt8Arr = randomArrObj.toArray() as? [UInt8] else {
            XCTFail("could not get uint 8 array from obj")
            return
        }
        
        let emptyUInt8Arr = [UInt8](repeating: 0, count: uInt8Arr.count)
        
        print(uInt8Arr)
        
        XCTAssertEqual(uInt8Arr.count, 10)
        XCTAssertNotEqual(emptyUInt8Arr, uInt8Arr)
        
        
        _ = runner.execute(name: contextId, code: "array = new Uint16Array(10); crypto.getRandomValues(array);")
        
        guard let randomArrObj = runner.execute(name: contextId, code: "array") else {
            XCTFail("array result is nil")
            return
        }
        
        guard let uInt16Arr = randomArrObj.toArray() as? [UInt16] else {
            XCTFail("could not get uint 16 array from obj")
            return
        }
        
        let emptyUInt16Arr = [UInt16](repeating: 0, count: uInt16Arr.count)
        
        print(uInt16Arr)
        
        XCTAssertEqual(uInt8Arr.count, 10)
        XCTAssertNotEqual(emptyUInt16Arr, uInt16Arr)
        
        _ = runner.execute(name: contextId, code: "array = new Uint32Array(10); crypto.getRandomValues(array);")
        
        guard let randomArrObj = runner.execute(name: contextId, code: "array") else {
            XCTFail("array result is nil")
            return
        }
        
        guard let uInt32Arr = randomArrObj.toArray() as? [UInt32] else {
            XCTFail("could not get uint 32 array from obj")
            return
        }
        
        let emptyUInt32Arr = [UInt32](repeating: 0, count: uInt32Arr.count)
        
        print(uInt32Arr)
        
        XCTAssertEqual(uInt32Arr.count, 10)
        XCTAssertNotEqual(emptyUInt32Arr, uInt32Arr)
    }
    
    func testFetch() throws {
        let expectation = XCTestExpectation(description: "Run callback set on event listener")
        
        let testCallback: @convention(block) () -> Void = {
            expectation.fulfill()
        }
        
        let code = """
            const fetchTodo = async () => {
                console.log("testing fetch");
                return fetch("https://jsonplaceholder.typicode.com/todos/1");
            }
        
            fetchTodo().then(async (res) => {
                console.log(res.url);
                console.log(await res.text());
                const todo = await res.json();
        
                console.log(todo.title);
                successCallback();
            }).catch((err) => {
                console.log("error");
                console.log(err);
            });
        """
        let contextId = "io.ionic.testFetch"
        let runner = Runner()
        let context = try runner.createContext(name: contextId)
        context.setGlobalObject(obj: testCallback, forName: "successCallback")
        _ = runner.execute(name: contextId, code: code)
        
        wait(for: [expectation], timeout: 10)        
    }
//    func testConsole() throws {
//        let runner = Runner()
//        try runner.run(sourcePath: nil, source: "console.log('hello world')")
//        try runner.run(sourcePath: nil, source: "console.warn('hello world')")
//        try runner.run(sourcePath: nil, source: "console.error('hello world')")
//    }
}
