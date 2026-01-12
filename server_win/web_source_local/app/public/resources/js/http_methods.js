let baseUrl = `http://${window.location.hostname}/`;
let getRequestHttp = function(url, endPoint, dataUrl, callBack) {
    var xhttp = new XMLHttpRequest();
    // console.log("GET"); //debug
    let request_status;
    xhttp.onreadystatechange = async function() {
        if (this.readyState == 4 && this.status == 200) {   
            response = this.responseText;
            callBack(this.responseText, this.status);
        }
        else 
        {
            callBack(this.responseText, this.status);
        }
    };
    xhttp.open("GET", url + endPoint + dataUrl, true);
    xhttp.send();
}

let delRequestHttp = function(url, endPoint, dataUrl, callBack) {
    var xhttp = new XMLHttpRequest();
    // console.log("GET"); //debug
    let request_status;
    xhttp.onreadystatechange = async function() {
        if (this.readyState == 4 && this.status == 200) {   
            response = this.responseText;
            callBack(this.responseText, this.status);
        }
        else 
        {
            callBack(this.responseText, this.status);
        }
    };
    xhttp.open("DELETE", url + endPoint + dataUrl, true);
    xhttp.send();
}

let postRequestHttp = function(url, endPoint, dataUrl, dataJson, callBack) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = async function() {
        if (this.readyState == 4 && this.status == 200) {
            response = this.responseText;
            // console.log(response); //debug
            callBack(this.responseText);
        }
    };
    xhttp.open("POST", url + endPoint + dataUrl, true);
    xhttp.setRequestHeader("Content-type", "application/json");
    xhttp.send(dataJson);
}

let putRequestHttp = function(url, endPoint, dataUrl, dataJson, callBack) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = async function() {
        if (this.readyState == 4 && this.status == 200) {
            response = this.responseText;
            // console.log(response); //debug
            callBack(this.responseText);
        }
    };
    xhttp.open("PUT", url + endPoint + dataUrl, true);
    xhttp.setRequestHeader("Content-type", "application/json");
    xhttp.send(dataJson);
}