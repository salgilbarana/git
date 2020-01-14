/*
 * 설정 파일

 */

// local -> shopping
// info 비우고 users3 -> users5 변경
// 스키마 객체를 coffeeshop 컬렉션 만들기 
// CoffeeShopSchema 와 CoffeeShopModel 속성 추가
module.exports = {
	server_port: 3000,	
	db_url: 'mongodb://localhost:27017/local',
	db_schemas: [
		{file:'./user_schema', collection:'users7', schemaName:'UserSchema', modelName:'UserModel'},
		{file:'./coffeeshop_schema', collection:'coffeeshop', schemaName:'CoffeeShopSchema', modelName:'CoffeeShopModel'}
	],
	route_info:[
		// 커피숍 추가 요청 // 커피숍 리스트 조회요청 
		{file: './coffeeshop', path:'/process/addcoffeeshop', method:'add', type:'post'}
		,{file: './coffeeshop', path:'/process/listcoffeeshop', method:'list', type:'post'}
		,{file: './coffeeshop', path:'/process/nearcoffeeshop', method:'findNear',type:'post'}
		,{file: './coffeeshop', path:'/process/withincoffeeshop', method:'findWithin', type:'post'}
		,{file: './coffeeshop', path:'/process/circlecoffeeshop', method:'findCircle', type:'post'}
		,{file: './coffeeshop', path:'/process/nearcoffeeshop2', method:'findNear2', type:'post'}
	],
	facebook: {		// passport facebook
		clientID: '1442860336022433',
		clientSecret: '13a40d84eb35f9f071b8f09de10ee734',
		callbackURL: 'http://localhost:3000/auth/facebook/callback'
	}
}