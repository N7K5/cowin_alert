const fetch = require('node-fetch');


function make_date() {
    const event = new Date();
    const options = { dateStyle: 'short' };
    const date_with_slash = event.toLocaleString('en', options);
    const m_d_y= date_with_slash.split("/");
    const m= m_d_y[0].length == 1? "0"+m_d_y[0]:m_d_y[0];
    const d= m_d_y[1].length == 1? "0"+m_d_y[1]:m_d_y[1];
    const y= m_d_y[2][0]+m_d_y[2][1]+m_d_y[2][2]+m_d_y[2][3];
    return d+"-"+m+"-"+y;
}




const alipurduar_dist_code= 710;
const coochbeher_dist_code= 783;

const url= "https://cdn-api.co-vin.in/api/v2/appointment/sessions/public/calendarByDistrict";






function get_data(district_id, date) {

    // let tokens= "?district_id=710&date=28-06-2021";
    let tokens= `?district_id=${district_id}&date=${date}`;

    return new Promise((resolve, reject) => {

        fetch(url+tokens, {
            headers: {
                // ":authority": "cdn-api.co-vin.in",
                "accept": "application/json, text/plain, */*"
            },
            method: "get",
        })
        .then(res =>{
            return res.json()
        })
        .then(res => {
            resolve(res);
        })
        .catch(e => {
            reject(e);
        })
    })
}



/*
format is
    every value is $ seperated
    ends with a $ too

    first is a character-
                1 => all okay  
                4 => some error happened, try again
                7 => fatal error, please see the logs and no tries again..


*/

function prepaer_data_for_esp32(dist_id) {

    if(dist_id == undefined) dist_id= alipurduar_dist_code;

    return new Promise((resolve, reject) => {
        let today= make_date();

        get_data(dist_id, today)
        .then(r => {
            
            let res= "1$";

            let available= 0;
            // for each centers
            r.centers.forEach(c => {
                // for each sessions in centers
                c.sessions.forEach(s => {
                    // console.log(s)
                    if(s.min_age_limit <  26 && s.available_capacity_dose1>0) {
                        // console.log(s.available_capacity_dose1);
                        available= 1;
                    }
                })
            });

            res= res+available+"$";
            resolve(res);

        })
        .catch(e => {
            reject("3$");
        })

    })

}


module.exports= {
    prepaer_data_for_esp32,
    alipurduar_dist_code,
    coochbeher_dist_code,
}