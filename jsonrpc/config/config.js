
// local -> shopping
// info 비우고 users3 -> users5 변경
// jsonrpc_api_path : '/api' 추가
module.exports = {
	server_port: 3000,	
	db_url: 'mongodb://localhost:27017/local',
	db_schemas: [
	    {file:'./user_schema', collection:'users5', schemaName:'UserSchema', modelName:'UserModel'}
	],
	route_info: [
	],
	facebook : {
		clientID:'1480459555440094',
		clientSecret:'c3f88c6c6304ef2ad22f95bfbbf56790',
		callbackURL:'/auth/facebook/callback'
	},
	jsonrpc_api_path : '/api'
}