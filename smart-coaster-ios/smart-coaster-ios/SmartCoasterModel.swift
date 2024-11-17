//
//  SmartCoasterModel.swift
//  smart-coaster-ios
//
//  Created by Vaaranan Yogalingam on 2024-11-17.
//

struct SmartCoaster : Codable {
    let id: Int
    var currentReading: Int
    var averageReading: Int
    var goal: Int
    var date: String
    var isGoalReached: Bool
}
