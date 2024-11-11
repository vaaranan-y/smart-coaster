//
//  Model.swift
//  smart-coaster-ios
//
//  Created by Vaaranan Yogalingam on 2024-11-10.
//

import SwiftUI

class SmartCoasterMode: ObservableObject {
    @Published var currentReading: Int = 0
    @Published var averageReading: Int = 0
    @Published var goal: Int = 0
    @Published var date: Date = Date()
    @Published var isGoalReached: Bool = false
}
