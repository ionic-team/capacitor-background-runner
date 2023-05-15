// swiftlint:disable line_length

import XCTest
import JavaScriptCore
@testable import RunnerEngine

final class ContextTests: XCTestCase {
    func testNullEval() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.nulleval")

        guard let value = context.execute(code: "undefined") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isUndefined)

        guard let value = context.execute(code: "const test = null; test;") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isNull)
    }

    func testBoolEval() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.booleval")

        guard let value = context.execute(code: "let test = (1 == 1); test;") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isBoolean)
        XCTAssertTrue(value.toBool())

        guard let value = context.execute(code: "test = (100 == 200); test;") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isBoolean)
        XCTAssertFalse(value.toBool())
    }

    func testIntegerEval() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.inteval")

        guard let value = context.execute(code: "1 +2;") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isNumber)
        XCTAssertEqual(3, value.toInt32())
    }

    func testFloatEval() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.dbleval")

        guard let value = context.execute(code: "10.8 + 2.77;") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isNumber)
        XCTAssertEqual(13.57, value.toDouble())
    }

    func testStringEval() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.stringeval")

        guard let value = context.execute(code: "'hello' + ' ' + 'world';") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isString)
        XCTAssertEqual("hello world", value.toString())
    }

    func testConsoleAPI() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.consoletests")

        _ = context.execute(code: "console.log('hello world');")
        _ = context.execute(code: "console.info('this message is for informational purposes');")
        _ = context.execute(code: "console.warn('this is a warning message');")
        _ = context.execute(code: "console.error('a problem has occurred');")
        _ = context.execute(code: "console.debug('this is a debugging statement');")
    }

    func testEventListeners() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.eventlistenertests")

        let expectation = XCTestExpectation(description: "Run callback on event")
        let expectation2 = XCTestExpectation(description: "Run another callback on event")
        let expectation3 = XCTestExpectation(description: "Run another callback on event with details")

        var successCallbackCalled = 0
        var altSuccessCallbackCalled = 0

        let successCallback: @convention(block) () -> Void = {
            successCallbackCalled += 1
            expectation.fulfill()
        }

        let successCallback2: @convention(block) () -> Void = {
            altSuccessCallbackCalled += 1
            expectation2.fulfill()
        }

        let successCallbackDetails: @convention(block) (JSValue) -> Void = {details in
            XCTAssertTrue(details.isObject)

            let detailsObj = details.toDictionary() as? [String: String]
            XCTAssertEqual(detailsObj?["name"], "John Doe")

            expectation3.fulfill()
        }

        context.ctx.setObject(successCallback, forKeyedSubscript: "successCallback" as NSString)
        context.ctx.setObject(successCallback2, forKeyedSubscript: "altSuccessCallback" as NSString)
        context.ctx.setObject(successCallbackDetails, forKeyedSubscript: "successCallbackDetails" as NSString)

        _ = context.execute(code: "addEventListener('myEvent', () => { successCallback(); });")
        try  context.dispatchEvent(event: "myEvent")

        wait(for: [expectation], timeout: 1)

        _ = context.execute(code: "addEventListener('myEvent', () => { altSuccessCallback(); });")
        try context.dispatchEvent(event: "myEvent")

        wait(for: [expectation2], timeout: 1)

        XCTAssertEqual(successCallbackCalled, 2)
        XCTAssertEqual(altSuccessCallbackCalled, 1)

        var detailsObject = [String: String]()
        detailsObject["name"] = "John Doe"

        _ = context.execute(code: "addEventListener('myEventDetails', (details) => { successCallbackDetails(details); });")
        try context.dispatchEvent(event: "myEventDetails", details: detailsObject)

        wait(for: [expectation3], timeout: 1)
    }

    func testCryptoAPI() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.cryptotests")

        guard let value = context.execute(code: "const array = new Uint32Array(10);  crypto.getRandomValues(array); array;") else {
            XCTFail("value is null")
            return
        }

        if value.isNull || value.isUndefined {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isObject)
        XCTAssertEqual(10, value.toDictionary().count)

        guard let value = context.execute(code: "crypto.randomUUID();") else {
            XCTFail("value is null")
            return
        }

        if value.isNull || value.isUndefined {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isString)
        XCTAssertEqual(36, value.toString().count)

    }

    func testTimeoutAPI() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.timeouttests")

        let expectation = XCTestExpectation(description: "Run callback set on timeout")
        let cancelExpectation = XCTestExpectation(description: "Cancel an already set up timeout")

        let successCallback: @convention(block) () -> Void = {
            expectation.fulfill()
        }

        let failureCallback: @convention(block) () -> Void = {
            XCTFail("callback was called and not canceled")
        }

        context.ctx.setObject(successCallback, forKeyedSubscript: "successCallback" as NSString)
        context.ctx.setObject(failureCallback, forKeyedSubscript: "failureCallback" as NSString)

        guard let value = context.execute(code: "setTimeout(() => { successCallback(); }, 2000)") else {
            XCTFail("value is null")
            return
        }

        XCTAssertGreaterThan(value.toInt32(), 0)

        wait(for: [expectation], timeout: 2.5)

        guard let value = context.execute(code: "setTimeout(() => { failureCallback(); }, 4000)") else {
            XCTFail("value is null")
            return
        }

        let timerId = value.toInt32()

        _ = Timer.scheduledTimer(withTimeInterval: TimeInterval(4.5), repeats: false) {_ in
            cancelExpectation.fulfill()
        }

        _ = context.execute(code: "clearTimeout(\(timerId))")

        wait(for: [cancelExpectation], timeout: 5)

    }

    func testIntervalAPI() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.intervaltests")

        var calls = 0

        let expectation = XCTestExpectation(description: "Run callback set on timeout")

        let timerCallback: @convention(block) () -> Void = {
            calls += 1
        }

        context.ctx.setObject(timerCallback, forKeyedSubscript: "timerCallback" as NSString)

        guard let value = context.execute(code: "setInterval(() => { timerCallback(); }, 2000)") else {
            XCTFail("value is null")
            return
        }

        let timerId = value.toInt32()

        _ = Timer.scheduledTimer(withTimeInterval: TimeInterval(8), repeats: false) {_ in
            _ = context.execute(code: "clearInterval(\(timerId))")
        }

        _ = Timer.scheduledTimer(withTimeInterval: TimeInterval(11), repeats: false) {_ in
            expectation.fulfill()
        }

        wait(for: [expectation], timeout: 12)

        XCTAssertEqual(calls, 4)

    }

    func testTextEncoderAPI() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.textencodertests")

        guard let value = context.execute(code: "const encoder = new TextEncoder(); encoder.encode('€');") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isObject)

        guard let arrayObject = value.toDictionary() as? [String: Int] else {
            XCTFail("could not get array object")
            return
        }

        XCTAssertEqual(226, arrayObject["0"])
        XCTAssertEqual(130, arrayObject["1"])
        XCTAssertEqual(172, arrayObject["2"])
    }

    func testTextDecoderAPI() throws {
        let runner = Runner()
        let context = try runner.createContext(name: "io.backgroundrunner.textdecodertests")

        guard let value = context.execute(code: "const win1251decoder = new TextDecoder(\"windows-1251\"); win1251decoder.decode(new Uint8Array([ 207, 240, 232, 226, 229, 242, 44, 32, 236, 232, 240, 33]));") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isString)
        XCTAssertEqual("Привет, мир!", value.toString())

        guard let value = context.execute(code: "const decoder = new TextDecoder(); decoder.decode(new Uint8Array([240, 160, 174, 183]));") else {
            XCTFail("value is null")
            return
        }

        XCTAssertTrue(value.isString)
        XCTAssertEqual("𠮷", value.toString())
    }
}

// swiftlint:enable line_length
