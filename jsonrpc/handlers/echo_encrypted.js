var crypto = require('crypto-js');

// params : 클라이언트로 전달받은 배열 객체
// callback : 클라이언트로 응답을 보낼떄
var echo = function(params, callback) {
    console.log('JSON-RPC echo 호출됨');
    console.dir(params);
    // null : 오류를 전달
    // params : 정상적인 데이터를 전달시 
    try{
        // 복호화 테스트
        var encrypted = params[0];
        var secret = 'my secret';
        var decrypted = crypto.AES.decrypt(encrypted, secret).toString(crypto.enc.Utf8);
        
        console.log('복호화된 데이터 : ' + decrypted);

        // 암호화 테스트
        var encrypted = '' + crypto.AES.encrypt(decrypted + '-> 서버에서 보냄', secret);

        console.log(encrypted);
        params[0] = encrypted;
    }catch(err){
        console.dir(err);
        console.log(err.stack);
    }
    
    callback(null, params);
};

module.exports = echo;