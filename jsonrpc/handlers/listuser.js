// 사용자 리스트 조회 함수
var listuser = function(params, callback){
    console.log("JSON-RPC listuser 호출됨.");
    console.dir(params);

    // 데이터베이스 객체 참조
    // database는 global.database 코드를 사용해 참조
   
    console.log(global.database);
    var database = global.database;
    if (database) {
        console.log('database 객체 참조됨.');
    }else{
        console.log('database 객체 불가함.');
        callback({
            code : 410,
            message : 'database 객체 불가함.'
        }, null);
        return;
    }

    if(database.db){
        // 1.모든 사용자 검색
        // UserModel : 사용자 정보를 담은 모델 객체
        database.UserModel.findAll(function(err, results){
        console.dir(results);

            if(err){
                callback({
                    code: 410,
                    message: err.message
                }, null);
                return ;
            }
            
            if(results){
                console.log('결과물 문서 데이터의 개수 : %d', results.length);

                var output = [];
                for (var i = 0; i< results.length; i++){
                    var curId = results[i]._doc.id;
                    var curName = results[i]._doc.name;
                    output.push({id : curId, name: curName});
                }
                console.dir(output);
                callback(null, output);
            }else{
                callback({
                    code : 410,
                    message: '사용자 리스트 조회 실패'
                }, null);
            }
        });
    }else{
        callback({
            code:410,
            message: '데이터베이스 연결 실패'
        }, null);
    }
};

module.exports = listuser;