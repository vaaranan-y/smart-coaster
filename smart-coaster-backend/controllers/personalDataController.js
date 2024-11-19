// Fetch user goal (hardcoded for now)
const getGoal = async (req, res) => {
    try {
        res.status(201).json({goal: 6});
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

module.exports = {getGoal};
