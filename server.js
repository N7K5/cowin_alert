const express = require('express')
const path = require('path')
const PORT = process.env.PORT || 5000

const app= express();

app.get("/", (req, res) =>{
    req.send("Well, working.......");
})

app.listen(PORT, () => console.log("listening to port"));