const express = require('express');
const path = require('path');
const PORT = process.env.PORT || 5000;


const {prepaer_data_for_esp32, coochbeher_dist_code, alipurduar_dist_code}= require("./cowin_fns");

let app= express();

app.use(express.static(path.join(__dirname, 'public')))

app.get("/cowin", (req, res) => {

  res.send("4$no_place");

})

app.get("/cowin/apd", (req, res) => {

  prepaer_data_for_esp32(alipurduar_dist_code)
  .then(data => {
    res.send(data);
  })
})

app.get("/cowin/cooch", (req, res) => {

  prepaer_data_for_esp32(coochbeher_dist_code)
  .then(data => {
    res.send(data);
  })
})

app.get('/*', (req, res) => {
  res.send("4$wrong_path");
})

app.listen(PORT, () => console.log(`Listening on ${ PORT }`))
