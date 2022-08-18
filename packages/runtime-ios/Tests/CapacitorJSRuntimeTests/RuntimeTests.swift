import XCTest
@testable import CapacitorJSRuntime

final class RuntimeTests: XCTestCase {
    func testConsole() throws {
        let runner = Runner()
        try runner.run(sourcePath: nil, source: "console.log('hello world')")
        try runner.run(sourcePath: nil, source: "console.warn('hello world')")
        try runner.run(sourcePath: nil, source: "console.error('hello world')")
    }
    
    func testCustomEvents() throws {
        let runner = Runner()
        try runner.run(sourcePath: nil, source: "const event = new CustomEvent('myEvent'); console.log(event.type)")
    }
    
    func testEventListeners() throws {
        let runner = Runner()
        try runner.run(sourcePath: nil, source: "addEventListener('push', () => { console.log('push event triggered'); });")
        
        runner.triggerEvent(event: "push")
    }
}
