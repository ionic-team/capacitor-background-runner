import Foundation

@objc public class BackgroundRunner: NSObject {
    @objc public func echo(_ value: String) -> String {
        print(value)
        return value
    }
}
