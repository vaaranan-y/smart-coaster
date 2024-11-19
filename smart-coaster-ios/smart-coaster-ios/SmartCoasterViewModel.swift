//
//  Model.swift
//  smart-coaster-ios
//
//  Created by Vaaranan Yogalingam on 2024-11-10.
//

import Combine
import Foundation

class SmartCoasterViewModel: ObservableObject {
    @Published var waterConsumedDay: Int?
    @Published var waterConsumedMonth: Int?
    @Published var waterGoal: Int?
    @Published var isLoading: Bool = false
    @Published var errorMessage: String?
    @Published var percentComplete: Float?
    
    private var cancellables = Set<AnyCancellable>()
    
    func fetchWaterConsumedDay() {
        self.isLoading = true
        self.errorMessage = nil
        
        let date = Date() // Get the current date and time
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "yyyy-MM-dd" 
        if let torontoTimeZone = TimeZone(identifier: "America/Toronto") {
            dateFormatter.timeZone = torontoTimeZone
        } else {
            print("Failed to set Toronto timezone")
        }

        let dateString = dateFormatter.string(from: date)
        
        let urlString = "http://localhost:8080/api/readings/day?dayStamp=\(dateString)"
        guard let url = URL(string: urlString) else {
            self.isLoading = false
            self.errorMessage = "Invalid URL"
            return
        }
        
        URLSession.shared.dataTask(with: url) {data, response, error in
            DispatchQueue.main.async {
                if let error = error {
                    self.errorMessage = "Error, unable to requested data: \(error.localizedDescription)"
                    self.isLoading = false
                    return
                }
                
                guard let data = data else {
                    self.errorMessage = "No data returned"
                    self.isLoading = false
                    return
                }
                
                // Parse response
                do {
                    let response = try JSONDecoder().decode(WaterConsumedDay.self, from: data)
                    self.waterConsumedDay = response.waterConsumedDay
                } catch {
                    self.errorMessage = "Failed to parse response: \(error.localizedDescription)"
                }
                self.isLoading = false
            }
        }.resume()
            
    }
        
    func fetchGoal() {
        self.isLoading = true
        self.errorMessage = nil
        
        let urlString = "http://localhost:8080/api/personalData/goal"
        guard let url = URL(string: urlString) else {
            self.isLoading = false
            self.errorMessage = "Invalid URL"
            return
        }
        
        URLSession.shared.dataTask(with: url) {data, response, error in
            DispatchQueue.main.async {
                if let error = error {
                    self.errorMessage = "Error, unable to requested data: \(error.localizedDescription)"
                    self.isLoading = false
                    return
                }
                
                guard let data = data else {
                    self.errorMessage = "No data returned"
                    self.isLoading = false
                    return
                }
                
                // Parse response
                do {
                    let response = try JSONDecoder().decode(WaterGoal.self, from: data)
                    self.waterGoal = response.goal
                } catch {
                    self.errorMessage = "Failed to parse response: \(error.localizedDescription)"
                }
                self.isLoading = false
            }
        }.resume()
    }
    
    func fetchPercentComplete() {
        // Fetch water consumed and goal concurrently
        fetchWaterConsumedDay()
        fetchGoal()
        
        // Use Combine to combine the values of both waterConsumedDay and waterGoal
        Publishers.CombineLatest($waterConsumedDay, $waterGoal)
            .sink { [weak self] waterConsumedDay, waterGoal in
                // Check if both values are non-nil
                if let waterConsumedDay = waterConsumedDay, let waterGoal = waterGoal {
                    self?.percentComplete = round(Float(waterConsumedDay) / Float(waterGoal) * 100)
                }
            }
            .store(in: &cancellables)
    }

}
