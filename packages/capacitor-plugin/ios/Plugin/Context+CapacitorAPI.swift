import Foundation

extension Context {
    public func setupCapacitorAPI() {
        jsContext.setObject(CapacitorKVStore.self, forKeyedSubscript: "CapacitorKV" as NSString)
        jsContext.setObject(CapacitorGeolocation(context: self), forKeyedSubscript: "CapacitorGeolocation" as NSString)
        jsContext.setObject(CapacitorWatch(), forKeyedSubscript: "CapacitorWatch" as NSString)
        jsContext.setObject(CapacitorNotifications.self, forKeyedSubscript: "CapacitorNotifications" as NSString)
        jsContext.setObject(CapacitorDevice(), forKeyedSubscript: "CapacitorDevice" as NSString)
        jsContext.setObject(CapacitorApp.self, forKeyedSubscript: "CapacitorApp" as NSString)
    }
}
