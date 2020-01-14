var Schema = {};

Schema.createSchema = function(mongoose){

    // 스키마 정의
    // name : 상호 이름
    // address : 주소
    // tel : 전화번호
    // geometry -> type : 정보의 유형
    // coordinates : 위치 좌표를 넣을 배열
    var CoffeeShopSchema = mongoose.Schema({
        name : {type : String, index: 'hashed', 'default': ''},
        address : {type : String, 'default': ''},
        tel : {type : String, 'default' : ''},
        geometry : {
            'type' : {type : String, 'default' : "Point"},
            coordinates : [{type : "Number"}]
        },
        created_at : {type: Date, index: {unique : false}, 'defualt':Date.now},
        updated_at : {type: Date, index: {unique : false}, 'defualt':Date.now}
    });
    
// 인덱스 만들어, 2dsphere 타입으로 지정
CoffeeShopSchema.index({geometry : '2dsphere'});

// 스키마에 Static() 메소드 추가
// 모든 커피숍 조회
// 라우팅하여 처리되는 파일 넣어도 됨 
CoffeeShopSchema.static('findAll', function(callback){
    return this.find({}, callback);
});

// 가까운 커피숍 조회
CoffeeShopSchema.static('findNear', function(longitude, latitude, maxDistance, callback){
    console.log('CoffeeShopSchema의 findNear 호출됨');
// near 메소드
// find().where(속성 이름).near(조건)
// parseFloat : 문자열을 숫자로
// limit(1) : 한개만 조회
    this.find().where('geometry').near(
        { center :{type : 'Point',
        coordinates:[parseFloat(longitude), parseFloat(latitude)]
    },
    maxDistance : maxDistance

        }).limit(1).exec(callback);
        });

// 일정 범위 안의 커피숍 조회
// within 메소드 : find().where(속성이름).within(조회 조건)
// parseFloat : 문자열을 숫자
CoffeeShopSchema.static('findWithin', function(topleft_longitude,topleft_latitude,
    bottomright_longitude, bottomright_latitude, callback){
        console.log('CoffeeShopSchema의 findWithin 호출됨.');

        this.find().where('geometry').within(
            {box : [[parseFloat(topleft_longitude), parseFloat(topleft_latitude)],
                [parseFloat(bottomright_longitude), parseFloat(bottomright_latitude)]]
        }).exec(callback);
    });

//일정 반경 안의 커피숍 조회
// radius : center속성과 기준점으로부터 거리
CoffeeShopSchema.static('findCircle',function(center_longitude, center_latitude, radius, 
    callback){
        console.log('CoffeeShopSchema의 findCircle 호출됨.');

        //change radian : 1/6371 -> 1km 
        this.find().where('geometry').within(
            {center:[parseFloat(center_longitude),parseFloat(center_latitude)],
        radius:parseFloat(radius/6371000),unique:true, spherical:true}).exec(callback);
    });

    console.log('CoffeeShopSchema 정의함.');

    return CoffeeShopSchema;
};



// module.exports에 UserSchema 객제 직접 전달
module.exports = Schema;