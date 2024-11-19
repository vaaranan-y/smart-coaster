//
//  ContentView.swift
//  smart-coaster-ios
//
//  Created by Vaaranan Yogalingam on 2024-11-09.
//

import SwiftUI
import Charts

struct ContentView: View {
    @StateObject private var viewModel = SmartCoasterViewModel()
    
    var defaultValue = -1
    var body: some View {
        VStack {
            HStack{
                Text("Smart Coaster")
                    .font(.system(size: 50))
                    .foregroundColor(.white)
                    .frame(maxWidth: .infinity, alignment: .center)
            }
            .frame(maxWidth: .infinity, alignment: .topLeading)
            .padding(.top, 20)
            VStack{
                Text("Total Cups Consumed: \(viewModel.waterConsumedDay ?? -1)")
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .padding(.top, 20)
                    .padding(.leading, 20)
                    .font(.system(size: 30))
                Text("Goal: \(viewModel.waterGoal ?? -1)")
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .padding(.top, 20)
                    .padding(.leading, 20)
                    .font(.system(size: 30))
                Text("% Complete:\(viewModel.percentComplete ?? -1)%")
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .padding(.top, 20)
                    .padding(.leading, 20)
                    .font(.system(size: 30))
            }.padding(.top, 40)
                .onAppear {
                    viewModel.fetchWaterConsumedDay()
                    viewModel.fetchGoal()
                    viewModel.fetchPercentComplete()
                }
            Spacer()
        }
        .containerRelativeFrame([.horizontal, .vertical])
        .background(Gradient(colors: [.blue, .white]))
    }
}

#Preview {
    ContentView()
}
