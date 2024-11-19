const express = require('express');
const router = express.Router();
const { getGoal } = require('../controllers/personalDataController');

// GET http://localhost:8080/api/personalData/goal
router.get('/goal', getGoal);

module.exports = router;
