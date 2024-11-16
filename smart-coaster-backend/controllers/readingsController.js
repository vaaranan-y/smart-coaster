const db = require('../config/firebase');

// Fetch all readings
const getReadings = async (req, res) => {
    try {
        const snapshot = await db.ref('userData/0').once('value');
        const readings = snapshot.val();
        console.log(readings);
        res.json(readings);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

// Fetch readings by TimeFrame
const getReadingsByDate = async (req, res) => {
    const { startStamp, endStamp } = req.query; // Timestamps must be in epoch time
    try {
        const snapshot = await db.ref('userData/0')
                                .orderByChild('timeStamp')
                                .startAt(startStamp)
                                .endAt(endStamp)
                                .once('value');
        const readings = snapshot.val();
        console.log(readings);
        res.json(readings);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

const getWaterConsumedOnDay = async (req, res) => {
    const { dayStamp } = req.query;

    const startOfDay = new Date(`${dayStamp}T00:00:00`);
    const endOfDay = new Date(`${dayStamp}T23:59:59`)

    const startStamp = Math.floor(startOfDay.getTime() / 1000).toString();
    const endStamp = Math.floor(endOfDay.getTime() / 1000).toString();

    try {
        const snapshot = await db.ref('userData/0')
                                .orderByChild('timeStamp')
                                .startAt(startStamp)
                                .endAt(endStamp)
                                .once('value');
        const readings = snapshot.val();

        masses = [];
        for(key in readings) {
            masses.push(parseFloat(readings[key].currentMass));
            console.log(parseFloat(readings[key].currentMass));
        }
 
        prevMass = masses[0];
        numberOfCupsDrank = 0;
        currFilledCupMass = masses[0];

        for(massIndex in masses) {
            mass = masses[massIndex];
            
            
            if(!((mass < prevMass + 5 && mass > prevMass - 5) || (mass <= 5))){
                console.log("Prev Mass: " + prevMass);
                console.log("Current Mass: " + mass);   
                // First condition checks if cup has not been picked up to drink
                // Second condition checks if cup has been picked up
                // If neither condition above has been fulfilled, then there was a definitive change in
                // the amount of water in the cup, which either means:
                //      1. The user took a sip of water
                //      2. The user refilled the cup

                

                if(mass > prevMass) {
                    // Cup has been set back down, and the mass has increased, meaning the cup was filled
                    console.log("Cup filled!");
                    numberOfCupsDrank += 1;
                    currFilledCupMass = mass;
                }
                prevMass = mass;
            }
        }

        res.json({"cupsDrank": numberOfCupsDrank});
    } catch (error) {
        res.status(500).json({ message: error.message });
    }


};

module.exports = {getReadings, getReadingsByDate, getWaterConsumedOnDay}