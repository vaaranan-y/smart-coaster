//
//  ContentView.swift
//  smart-coaster-ios
//
//  Created by Vaaranan Yogalingam on 2024-11-09.
//

import SwiftUI
import Charts



struct DropDown: View {
    @State public var selectedUnit: String = "cups"
    
    
    var units = ["mL", "cups"]
    
    var body: some View {
        VStack {
            Menu("Units: \(selectedUnit)"){
                ForEach(units, id: \.self){ unit in
                    Button(unit, action:{
                        selectedUnit = unit
                    })
                }
            }.font(.title3)
        }
        .frame(maxWidth: 150, maxHeight: 50, alignment: .center)
                .background(
                    RoundedRectangle(cornerRadius: 10).fill(Color(UIColor.white).opacity(0.9))
                )
    }
}

struct ContentView: View {
    @StateObject private var viewModel = SmartCoasterViewModel()
    
    var defaultValue = -1
    var body: some View {
        VStack {
            CardView(title: "Cups Drank", value: "5"),
            HStack{
                Text("Smart Coaster")
                    .font(.system(size: 50))
                    .foregroundColor(.white)
                    .frame(maxWidth: .infinity, alignment: .center)
            }
            
            .frame(maxWidth: .infinity, alignment: .topLeading)
            .padding(.top, 20)
            DropDown()
            VStack{
                HStack{
                    Text("Total Cups Consumed: \(viewModel.waterConsumedDay ?? -1)")
                        .foregroundColor(Color(hue: 0.677, saturation: 1.0, brightness: 0.401))
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding(.top, 20)
                        .padding(.leading, 20)
                        .font(.system(size: 30))
                }
                
                HStack {
                    Text("Goal: \(viewModel.waterGoal ?? -1)")
                        .foregroundColor(Color(hue: 0.677, saturation: 1.0, brightness: 0.401))
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding(.top, 20)
                        .padding(.leading, 20)
                        .font(.system(size: 30))
                }
                HStack {
                    Text("% Complete:\(viewModel.percentComplete ?? -1)%")
                        .foregroundColor(Color(hue: 0.677, saturation: 1.0, brightness: 0.401))
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .padding(.top, 20)
                        .padding(.leading, 20)
                        .font(.system(size: 30))
                }
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
