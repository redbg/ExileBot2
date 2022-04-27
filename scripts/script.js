
const SocketState = {
    Unconnected: 0,
    HostLookup: 1,
    Connecting: 2,
    Connected: 3,
    Bound: 4,
    Listening: 5,
    Closing: 6,
}

const CharacterClassType = {
    StrDexInt: 0,
    Str: 1,
    Dex: 2,
    Int: 3,
    StrDex: 4,
    StrInt: 5,
    DexInt: 6,
}

function Tick() {
    if (Client.SocketState == SocketState.Unconnected && Game.SocketState == SocketState.Unconnected) {
        Client.connectToHost("sjc01.login.pathofexile.com", 20481);
    }

    if (Game.SocketState == SocketState.Connected) {
        // console.log(JSON.stringify(Game.EntityList));
        // console.log(JSON.stringify(Game.ItemList));

        // 打印坐标
        // for (let index = 0; index < Game.EntityList.length; index++) {
        //     const element = Game.EntityList[index];
        //     console.log(element.Pos);
        // }
    }
}

// 响应登录成功事件
function OnClientLoginSuccess() {
    console.log("OnClientLoginSuccess" + " " + JSON.stringify(Client));
}

// 响应收到角色列表事件
function OnClientCharacterList() {
    console.log("OnClientCharacterList");

    var array = Client.CharacterList;

    for (let i = 0; i < array.length; i++) {
        const character = array[i];
        console.log("[" + i + "]" + " " + JSON.stringify(character));
    }

    if (array.length > 0) {
        // 选择角色,进入游戏
        Client.SendSelectCharacter(0);
    }
    else {
        // 创建角色
        Client.SendCreateCharacter(randomString(Math.floor(Math.random() * 8) + 8), "Archnemesis", CharacterClassType.Int);
    }
}

// 生成随机字符串
function randomString(len = 10) {
    var result = '';
    var chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (let i = 0; i < len; i++) {
        result += chars.charAt(Math.floor(Math.random() * chars.length));
    }

    return result;
}