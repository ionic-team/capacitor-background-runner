import XCTest
@testable import CapacitorJSRuntime

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
            context.emitEvent(event: "push")
        }
        
        wait(for: [expectation], timeout: 10.0)
        
    }
    
//    func testConsole() throws {
//        let runner = Runner()
//        try runner.run(sourcePath: nil, source: "console.log('hello world')")
//        try runner.run(sourcePath: nil, source: "console.warn('hello world')")
//        try runner.run(sourcePath: nil, source: "console.error('hello world')")
//    }
}
