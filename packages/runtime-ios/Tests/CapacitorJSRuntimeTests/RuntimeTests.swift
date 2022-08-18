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
//    func testConsole() throws {
//        let runner = Runner()
//        try runner.run(sourcePath: nil, source: "console.log('hello world')")
//        try runner.run(sourcePath: nil, source: "console.warn('hello world')")
//        try runner.run(sourcePath: nil, source: "console.error('hello world')")
//    }
//
//    func testCustomEvents() throws {
//        let runner = Runner()
//        try runner.run(sourcePath: nil, source: "const event = new CustomEvent('myEvent'); console.log(event.type)")
//    }
//
//    func testEventListeners() throws {
//        let runner = Runner()
//        try runner.run(sourcePath: nil, source: "addEventListener('push', () => { console.log('push event triggered'); });")
//
//        runner.triggerEvent(event: "push")
//    }
//
//    func testURL() throws {
//
//    }
}
