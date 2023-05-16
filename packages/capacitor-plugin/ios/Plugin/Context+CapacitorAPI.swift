import Foundation

extension Context {
    public func setupCapacitorAPI() {
        ctx.setObject(CapacitorKVStore.self, forKeyedSubscript: "CapacitorKV" as NSString)
        ctx.setObject(CapacitorGeolocation(context: self), forKeyedSubscript: "CapacitorGeolocation" as NSString)
        ctx.setObject(CapacitorWearable(context: self.ctx), forKeyedSubscript: "CapacitorWearable" as NSString)
        ctx.setObject(CapacitorNotifications.self, forKeyedSubscript: "CapacitorNotifications" as NSString)
    }
}
