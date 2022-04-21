function tick() {
    console.log("===========");
    for (var i in Client) {
        console.log(i, " ", typeof (Client[i]));
    }
}