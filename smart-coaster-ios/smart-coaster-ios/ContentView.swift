//
//  ContentView.swift
//  smart-coaster-ios
//
//  Created by Vaaranan Yogalingam on 2024-11-09.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack {
            HStack{
                Text("Smart Coaster")
                    .font(.largeTitle)
                    .font(Font.custom("LexendDeca-Bold", size: 18))
                    .foregroundColor(.white)
                    .frame(maxWidth: .infinity, alignment: .center)
            }
            .frame(maxWidth: .infinity, alignment: .topLeading)
            .padding(.top, 20)
        Spacer()
            
        }
        .containerRelativeFrame([.horizontal, .vertical])
        .background(Gradient(colors: [.blue, .white]))
    }
}

#Preview {
    ContentView()
}
