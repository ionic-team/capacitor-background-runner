import Foundation
import OSLog

class ContextLogger {
    public let execute: Logger
    public let fetch: Logger
    public let console: Logger
    
    init(name: String) {
        let subsystem = "JSEngine[\(name)]"
        
        execute = Logger(subsystem: subsystem, category: "Execute")
        fetch = Logger(subsystem: subsystem, category: "Fetch")
        console = Logger(subsystem: subsystem, category: "Console")
    }
}
