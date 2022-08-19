import Foundation

@objc public class CapacitorNativeWorker: NSObject {
    @objc public func echo(_ value: String) -> String {
        print(value)
        return value
    }
}
