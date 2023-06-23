//
//  WatchViewModel.swift
//  WatchDemo Watch App
//
//  Created by Joseph Pender on 5/22/23.
//

import Foundation
import WatchConnectivity

class WatchViewModel: NSObject, WCSessionDelegate {
    override init() {
        super.init()
        
        WCSession.default.delegate = self
        WCSession.default.activate()
    }
    
    func sendMessageToPhone() {
        var message: [String: Any] = [:]
        message["result"] = "This is a message from the watch"
        
        WCSession.default.sendMessage(message, replyHandler: nil)
        WCSession.default.transferUserInfo(message)
    }
    
    func session(_ session: WCSession, activationDidCompleteWith activationState: WCSessionActivationState, error: Error?) {
        print("watch activation is complete")
    }
    
    func session(_ session: WCSession, didReceiveMessage message: [String : Any]) {
        print("Received Phone Message")
        print(message)
    }
    
    func session(_ session: WCSession, didReceiveUserInfo userInfo: [String : Any] = [:]) {
        print("Received user info")
        print(userInfo)
    }
}
