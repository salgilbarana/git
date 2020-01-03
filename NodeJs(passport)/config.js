/*
 * 설정 파일
 *
 * @date 2016-11-10
 * @author Mike
 */

// local -> shopping
// info 비우고 users3 -> users5 변경
module.exports = {
	server_port: 3000,	
	db_url: 'mongodb://localhost:27017/local',
	db_schemas: [
	    {file:'./user_schema', collection:'users5', schemaName:'UserSchema', modelName:'UserModel'}
	],
	route_info: [
	]
}