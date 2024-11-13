// Application config
const express = require('express');
const app = express();

// Middleware
app.use(express.json());

// Routes
app.use('/api/readings', require('./routes/readings'));

module.exports = app;