import Foundation
import JavaScriptCore

@objc protocol JSURLExports: JSExport {
    var hash: String { get set }
    var host: String { get set }
    var hostname: String { get set }
    var href: String { get set }
    var origin: String { get }
    var password: String { get set }
    var pathname: String { get set }
    var port: String { get set }
    var search: String { get set }
    var username: String { get set }
    
    func toString() -> String
    func toJSON() -> String
}
