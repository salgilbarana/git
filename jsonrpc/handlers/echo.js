// params : 클라이언트로 전달받은 배열 객체
// callback : 클라이언트로 응답을 보낼떄
var echo = function(params, callback) {
    console.log('JSON-RPC echo 호출됨');
    console.dir(params);
    // null : 오류를 전달
    // params : 정상적인 데이터를 전달시 
    callback(null, params);
};

module.exports = echo;