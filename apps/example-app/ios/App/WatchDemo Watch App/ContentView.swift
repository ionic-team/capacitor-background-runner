//
//  ContentView.swift
//  WatchDemo Watch App
//
//  Created by Joseph Pender on 5/22/23.
//

import SwiftUI

struct ContentView: View {
    var model = WatchViewModel()
    
    var body: some View {
        VStack {
            Image(systemName: "globe")
                .imageScale(.large)
                .foregroundColor(.accentColor)
            Text("Hello, world!")
            Button {
                model.sendMessageToPhone()
            } label: {
                Text("Send Message")
            }
        }
        .padding()
        
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
