/**
 * 데이터베이스 스키마를 정의하는 모듈
 */

var crypto = require('crypto');

var Schema = {};

Schema.createSchema = function(mongoose) {
	
	// 스키마 정의
	// 한줄 삭제 쉽게 하기 위해 앞에 , 
	var UserSchema = mongoose.Schema({
	    email : {type: String,'default':''}
		,hashed_password: {type: String, required: true, 'default':''}
		,name: {type: String, index: 'hashed', 'default':''}
	    ,salt: {type:String}
	    ,created_at: {type: Date, index: {unique: false}, 'default': Date.now}
		,updated_at: {type: Date, index: {unique: false}, 'default': Date.now}
		,provider : {type: String, 'default' : ''} //사용자 인증서비스를 제공하는 제공자 이름
		,authToken : {type: String, 'default' : ''} // 인증서버에서 제공하는 access token값
		,facebook : {} // 응답받은 사용자 정보 객체
	});
	
	// password를 virtual 메소드로 정의 : MongoDB에 저장되지 않는 편리한 속성임. 특정 속성을 지정하고 set, get 메소드를 정의함
	UserSchema
	  .virtual('password')
	  .set(function(password) {
	    this._password = password;
	    this.salt = this.makeSalt();
	    this.hashed_password = this.encryptPassword(password);
	    console.log('virtual password 호출됨 : ' + this.hashed_password);
	  })
	  .get(function() { return this._password });
	
	// 스키마에 모델 인스턴스에서 사용할 수 있는 메소드 추가
	// 비밀번호 암호화 메소드
	UserSchema.method('encryptPassword', function(plainText, inSalt) {
		if (inSalt) {
			return crypto.createHmac('sha1', inSalt).update(plainText).digest('hex');
		} else {
			return crypto.createHmac('sha1', this.salt).update(plainText).digest('hex');
		}
	});
	
	// salt 값 만들기 메소드
	UserSchema.method('makeSalt', function() {
		return Math.round((new Date().valueOf() * Math.random())) + '';
	});
	
	// 인증 메소드 - 입력된 비밀번호와 비교 (true/false 리턴)
	UserSchema.method('authenticate', function(plainText, inSalt, hashed_password) {
		if (inSalt) {
			console.log('authenticate 호출됨 : %s -> %s : %s', plainText, this.encryptPassword(plainText, inSalt), hashed_password);
			return this.encryptPassword(plainText, inSalt) === hashed_password;
		} else {
			console.log('authenticate 호출됨 : %s -> %s : %s', plainText, this.encryptPassword(plainText), this.hashed_password);
			return this.encryptPassword(plainText) === this.hashed_password;
		}
	});
	
	// 값이 유효한지 확인하는 함수 정의
	var validatePresenceOf = function(value) {
		return value && value.length;
	};
		
	// 저장 시의 트리거 함수 정의 (password 필드가 유효하지 않으면 에러 발생)
	UserSchema.pre('save', function(next) {
		if (!this.isNew) return next();
	
		if (!validatePresenceOf(this.password)) {
			next(new Error('유효하지 않은 password 필드입니다.'));
		} else {
			next();
		}
	})
	
	// 필수 속성에 대한 유효성 확인 (길이값 체크) 
	// path 메소드 : 칼럼을 값을 확인 하는데 사용
	// validate : 값이 유효한지 확인

	// id 삭제
	
	// name 삭제

	UserSchema.path('email').validate(function (email) {
		return email.length;
	}, 'email 칼럼의 값이 없습니다.');
	
	//UserSchema.path('hashed_password').validate(function (hashed_password) {
	//	return hashed_password.length;
	//}, 'hashed_password 칼럼의 값이 없습니다.');
	
	   
	// 스키마에 static 메소드 추가

	// findById -> findByemail 수정

	UserSchema.static('findbyEmail', function(id, callback) {
		return this.find({email:email}, callback);
	});
	
	UserSchema.static('findAll', function(callback) {
		return this.find({}, callback);
	});
	
	console.log('UserSchema 정의함.');

	return UserSchema;
};

// module.exports에 UserSchema 객체 직접 할당
module.exports = Schema;

