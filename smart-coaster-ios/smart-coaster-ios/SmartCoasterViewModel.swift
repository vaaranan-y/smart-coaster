//
//  Model.swift
//  smart-coaster-ios
//
//  Created by Vaaranan Yogalingam on 2024-11-10.
//

import SwiftUI
import Combine

class SmartCoasterViewModel: ObservableObject {
    @Published var smartCoaster: SmartCoaster?
    @Published var isLoading: Bool = false
    @Published var errorMessage: String?
    
    private var cancellables = Set<AnyCancellable>()
    
    init(smartCoaster: SmartCoaster? = SmartCoaster(id: 0, currentReading: 0, averageReading: 0, goal: 0, date: "", isGoalReached: false), isLoading: Bool, errorMessage: String? = nil, cancellables: Set<AnyCancellable> = Set<AnyCancellable>()) {
        self.smartCoaster = smartCoaster
        self.isLoading = isLoading
        self.errorMessage = errorMessage
        self.cancellables = cancellables
    }

}
