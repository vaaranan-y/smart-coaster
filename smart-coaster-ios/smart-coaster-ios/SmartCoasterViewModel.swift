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
    @Published var isLoading: Bool = false
    @Published var errorMessage: String?
    
    private var cancellables = Set<AnyCancellable>()
    
    func fetchWaterConsumedDay() {
        self.isLoading = true
        self.errorMessage = nil
        
        let urlString = "http://localhost:8080/api/readings/day?dayStamp=2024-11-16"
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
        

}
