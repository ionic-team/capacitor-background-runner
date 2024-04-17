// swiftlint:disable all
import XCTest
@testable import RunnerEngine

final class RunnerTests: XCTestCase {
    func testMultipleContexts() throws {
        let runner = Runner()

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
    }

}
// swiftlint:enable all
