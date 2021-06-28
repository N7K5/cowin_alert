function make_date() {
    const event = new Date();
    const options = { dateStyle: 'short' };
    const date_with_slash = event.toLocaleString('en', options);
    const m_d_y= date_with_slash.split("/");
    const m= m_d_y[0].length == 1? "0"+m_d_y[0]:m_d_y[0];
    const d= m_d_y[1].length == 1? "0"+m_d_y[1]:m_d_y[1];
    const y= m_d_y[2].length == 2? "20"+m_d_y[2]:m_d_y[2];
    return d+"-"+m+"-"+y;
}







const alipurduar_dist_code= 710;
const url= "https://cdn-api.co-vin.in/api/v2/appointment/sessions/public/calendarByDistrict";

const today= make_date();






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


get_data(alipurduar_dist_code, today)
.then(r => {
    console.log(r);
    document.body.innerText= JSON.stringify(r);
})
.catch(e => {
    console.log(e);
})

function preaper_page() {
    get_data(alipurduar_dist_code, today)
}