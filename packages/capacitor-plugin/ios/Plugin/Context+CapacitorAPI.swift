import Foundation

extension Context {
    public func setupCapacitorAPI() {
        self.ctx.setObject(CapacitorKVStore.self, forKeyedSubscript: "CapacitorKV" as NSString)
        self.ctx.setObject(CapacitorGeolocation(), forKeyedSubscript: "CapacitorGeolocation" as NSString)
        self.ctx.setObject(CapacitorWearable(context: self.ctx), forKeyedSubscript: "CapacitorWearable" as NSString)
    }
}
