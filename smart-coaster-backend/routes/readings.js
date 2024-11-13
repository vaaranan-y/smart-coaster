const express = require('express');
const router = express.Router();

router.get('/', (req, res) => {
    res.send('Return all readings here');
})

router.get('/average', (req, res) => {
    res.send('Return average readging here');
})

module.exports = router;