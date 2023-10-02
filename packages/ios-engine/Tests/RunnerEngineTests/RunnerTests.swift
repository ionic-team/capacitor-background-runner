// swiftlint:disable all
import XCTest
@testable import RunnerEngine

final class RunnerTests: XCTestCase {
    func testRunnerCreateDestroy() {
        let runner = Runner()
        runner.start()
        
        runner.stop()
    }
    
    func testContextCreateDestroy() throws {
        let runner = Runner()
        runner.start()
        
        _ = try runner.createContext(name: "io.ionic.android_js_engine")
        
        runner.stop()
    }
    
    func testMultipleContexts() throws {
        let runner = Runner()
        runner.start()
        
        var calls = 0
        let count = Int.random(in: 20..<40)
        
        let expectation = XCTestExpectation(description: "Run callback on completed timeout callbacks")
        
        let timeoutCallback: @convention(block) () -> Void = {
            calls += 1
            
            if calls >= count {
                expectation.fulfill()
            }
        }
        
        for i in 1...count {
            let context = try runner.createContext(name: "context #\(i)")
            context.jsContext.setObject(timeoutCallback, forKeyedSubscript: "timeoutCallback" as NSString)
            _ = try context.execute(code: "setTimeout(() => { timeoutCallback(); }, 2000)")
        }
        
        wait(for: [expectation], timeout: 5)
        
        runner.stop()
    }

}
// swiftlint:enable all
