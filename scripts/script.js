/** ExileClient.SocketState
 * 
 * UnconnectedState
 * HostLookupState
 * ConnectingState
 * ConnectedState
 * BoundState
 * ListeningState
 * ClosingState
 */

/** Character.ClassType
 * 
 * StrDexInt // 貴族
 * Str       // 野蠻人    特性：力量，近戰
 * Dex       // 遊俠      特性：敏捷，遠距離
 * Int       // 女巫      特性：智慧，法術
 * StrDex    // 決鬥者
 * StrInt    // 聖堂武僧
 * DexInt    // 暗影刺客
 */

function Tick() {

    if (Client.SocketState == ExileClient.UnconnectedState) {
        Client.connectToHost("sjc01.login.pathofexile.com", 20481);
    } else if (Client.SocketState == ExileClient.ConnectedState) {

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
        Client.SendCreateCharacter(randomString(Math.floor(Math.random() * 8) + 8), "Archnemesis", Character.Int);
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