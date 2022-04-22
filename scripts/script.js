/** SocketState
 * UnconnectedState
 * HostLookupState
 * ConnectingState
 * ConnectedState
 * BoundState
 * ListeningState
 * ClosingState
 */

function Tick() {
    if (Client.SocketState == ExileClient.UnconnectedState) {
        Client.connectToHost("sjc01.login.pathofexile.com", 20481);
    } else if (Client.SocketState == ExileClient.ConnectedState) {

    }
}

function OnClientLoginSuccess() {
    console.log("OnClientLoginSuccess");
    console.log("AccountName:" + Client.AccountName);
}

function OnClientCharacterList() {
    console.log("OnClientCharacterList");

    var array = Client.CharacterList;

    for (let i = 0; i < array.length; i++) {
        const element = array[i];
        console.log(JSON.stringify(element));
    }
}
