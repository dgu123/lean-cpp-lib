/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CONTAINERS_SIMPLE_HASH_MAP
#define LEAN_CONTAINERS_SIMPLE_HASH_MAP

#include "../lean.h"
#include "../smart/terminate_guard.h"
#include "../tags/noncopyable.h"
#include "../functional/hashing.h"
#include <memory>
#include <utility>
#include <functional>
#include <algorithm>
#include <limits>
#include <string>

namespace lean 
{
namespace containers
{

/// Defines construction policies for the class simple_hash_map.
namespace simple_hash_map_policies
{
	/// Simple hash map element construction policy.
	template <bool RawMove = false, bool NoDestruct = false, bool RawKeyMove = RawMove, bool NoKeyDestruct = NoDestruct>
	struct policy
	{
		/// Specifies whether memory containing constructed elements may be moved as a whole, without invoking the contained elements' copy or move constructors.
		static const bool raw_move = RawMove;
		/// Specifies whether memory containing constructed elements may be freed as a whole, without invoking the contained elements' destructors.
		static const bool no_destruct = NoDestruct;
		/// Specifies whether memory containing constructed keys may be moved as a whole, without invoking the contained keys' copy or move constructors.
		static const bool raw_key_move = RawKeyMove;
		/// Specifies whether memory containing constructed keys may be freed as a whole, without invoking the contained keys' destructors.
		static const bool no_key_destruct = NoKeyDestruct;
	};

	/// Default element construction policy.
	typedef policy<> nonpod;
	/// Semi-POD key construction policy (raw move, yet proper destruction).
	typedef policy<false, false, true> semipodkey;
	/// Semi-POD element construction policy (raw move, yet proper destruction).
	typedef policy<true> semipod;
	/// POD key construction policy.
	typedef policy<false, false, true, true> podkey;
	/// POD key / Semi-POD element construction policy.
	typedef policy<true, false, true, true> podkey_semipod;
	/// POD element construction policy.
	typedef policy<true, true> pod;
}

namespace impl
{
	template <class Key>
	struct default_numeric_keys
	{
		LEAN_STATIC_ASSERT_MSG_ALT(std::numeric_limits<Key>::is_specialized,
			"No invalid key default available for the given type.",
			No_invalid_key_default_available_for_the_given_type);

		// WORKAROUND: Using methods here circumvents compiler bug that won't initialize some static constant variables
		// when these are copied from to initialize other constant static variables
		LEAN_INLINE static Key invalid_key()
		{
			return (std::numeric_limits<Key>::has_infinity)
				? std::numeric_limits<Key>::infinity()
				: (std::numeric_limits<Key>::min() != Key())
					? std::numeric_limits<Key>::min()
					: std::numeric_limits<Key>::max();
		}
		LEAN_INLINE static Key end_key()
		{
			return Key();
		}
	};

	template <class Key>
	struct default_numeric_keys<Key*>
	{
		LEAN_INLINE static Key* invalid_key()
		{
			return nullptr;
		}
		LEAN_INLINE static Key* end_key()
		{
			return reinterpret_cast<Key*>( static_cast<uintptr_t>(-1) );
		}
	};
}

/// Defines default values for invalid & end keys.
template <class Key>
struct default_keys
{
	/// Invalid key value that is guaranteed never to be used in key-value-pairs.
	static const Key invalid_key;
	/// Valid key value used as end marker. May still be used in actual key-value-pairs.
	static const Key end_key;
	/// Predicate used in key validity checks.
	struct key_equal
	{
		/// Predicate.
		static const std::equal_to<Key> predicate;		
	};
};

// Numeric / generic defaults
template <class Key> const Key default_keys<Key>::invalid_key = impl::default_numeric_keys<Key>::invalid_key();
template <class Key> const Key default_keys<Key>::end_key = impl::default_numeric_keys<Key>::end_key();
template <class Key> const std::equal_to<Key> default_keys<Key>::key_equal::predicate = std::equal_to<Key>();

template <class Char, class Traits, class Allocator>
struct default_keys< std::basic_string<Char, Traits, Allocator> >
{
	typedef std::basic_string<Char, Traits, Allocator> key_type;

	static const key_type invalid_key;
	static const key_type end_key;
	struct key_equal
	{
		static const std::equal_to<key_type> predicate;		
	};
};

// String defaults
template <class Char, class Traits, class Allocator>
const std::basic_string<Char, Traits, Allocator>
	default_keys< std::basic_string<Char, Traits, Allocator> >::invalid_key = std::basic_string<Char, Traits, Allocator>();
template <class Char, class Traits, class Allocator>
const std::basic_string<Char, Traits, Allocator>
	default_keys< std::basic_string<Char, Traits, Allocator> >::end_key = std::basic_string<Char, Traits, Allocator>(1, Char());

namespace impl
{

/// Gets the first prime number available that is both less than or equal to the given maximum value
/// and greater than or equal to the given capacity, if possible.
inline size_t next_prime_capacity(size_t capacity, size_t max)
{
	// Prime numbers growing by 8%
	static const uint4 primes[] = {
		2U, 3U, 5U, 7U, 11U, 13U, 17U, 19U, 23U, 29U, 31U,
		37U, 41U, 43U, 47U, 53U, 59U, 61U, 67U, 71U, 73U, 79U,     
		83U, 89U, 97U, 103U, 109U, 113U, 127U, 137U, 139U, 149U,     
		157U, 167U, 179U, 193U, 199U, 211U, 227U, 241U, 257U,     
		277U, 293U, 313U, 337U, 359U, 383U, 409U, 439U, 467U,   
		503U, 541U, 577U, 619U, 661U, 709U, 761U, 823U, 887U,    
		953U, 1031U, 1109U, 1193U, 1289U, 1381U, 1493U, 1613U,     
		1741U, 1879U, 2029U, 2179U, 2357U, 2549U, 2753U, 2971U,    
		3209U, 3469U, 3739U, 4027U, 4349U, 4703U, 5087U, 5503U,    
		5953U, 6427U, 6949U, 7517U, 8123U, 8783U, 9497U, 10273U,    
		11113U, 12011U, 12983U, 14033U, 15173U, 16411U, 17749U,    
		19183U, 20753U, 22447U, 24281U, 26267U, 28411U, 30727U,    
		33223U, 35933U, 38873U, 42043U, 45481U, 49201U, 53201U,      
		57557U, 62233U, 67307U, 72817U, 78779U, 85229U, 92203U,     
		99733U, 107897U, 116731U, 126271U, 136607U, 147793U,     
		159871U, 172933U, 187091U, 202409U, 218971U, 236897U,    
		256279U, 277261U, 299951U, 324503U, 351061U, 379787U,    
		410857U, 444487U, 480881U, 520241U, 562841U, 608903U,     
		658753U, 712697U, 771049U, 834181U, 902483U, 976369U,     
		1056323U, 1142821U, 1236397U, 1337629U, 1447153U, 1565659U, 
		1693859U, 1832561U, 1982627U, 2144977U, 2320627U, 2510653U,    
		2716249U, 2938679U, 3179303U, 3439651U, 3721303U, 4026031U,     
		4355707U, 4712381U, 5098259U, 5515729U, 5967347U, 6456007U,     
		6984629U, 7556579U, 8175383U, 8844859U, 9569143U, 10352717U,     
		11200489U, 12117689U, 13109983U, 14183539U, 15345007U,       
		16601593U, 17961079U, 19431899U, 21023161U, 22744717U,     
		24607243U, 26622317U, 28802401U, 31160981U, 33712729U,    
		36473443U, 39460231U, 42691603U, 46187573U, 49969847U,     
		54061849U, 58488943U, 63278561U, 68460391U, 74066549U,    
		80131819U, 86693767U, 93793069U, 101473717U, 109783337U,   
		118773397U, 128499677U, 139022417U, 150406843U, 162723577U,  
		176048909U, 190465427U, 206062531U, 222936881U, 241193053U,    
		260944219U, 282312799U, 305431229U, 330442829U, 357502601U,    
		386778277U, 418451333U, 452718089U, 489790921U, 529899637U,    
		573292817U, 620239453U, 671030513U, 725980837U, 785430967U,     
		849749479U, 919334987U, 994618837U, 1076067617U, 1164186217U,   
		1259520799U, 1362662261U, 1474249943U, 1594975441U,       
		1725587117U, 1866894511U, 2019773507U, 2185171673U,     
		2364114217U, 2557710269U, 2767159799U, 2993761039U,      
		3238918481U, 3504151727U, 3791104843U, 4101556399U,
		4294967291U,

		// Sentinel
		4294967291U
	};
	// Exclude sentinel, thus value returned by lower_bound is always valid.
	static const size_t primeCount = sizeof(primes) / sizeof(primes[0]) - 1U;

	// Smallest prime number.
	static const uint4 primeThreshold = 2U;
	// Largest prime number in 4 byte unsigned integer.
	static const uint4 largePrimeThreshold = 4294967291U;

	// 32 bit size or request <= primeThreshold
	if (sizeof(size_t) <= sizeof(uint4) || capacity <= largePrimeThreshold || max <= largePrimeThreshold)
	{
		const uint4 *primeCapacity = std::lower_bound(primes, primes + primeCount, static_cast<uint4>(capacity));
		
		LEAN_ASSERT(max >= primeThreshold);

		while (*primeCapacity > max)
			--primeCapacity;

		LEAN_ASSERT(primeCapacity >= primes);
		LEAN_ASSERT(*primeCapacity <= max);

		// Legal as max is of type size_t
		return static_cast<size_t>(*primeCapacity);
	}
	else
	{
		// More prime numbers growing by 8%
		static const uint8 largePrimes[] = {
			// Sentinel
			largePrimeThreshold,

			4429680911ULL, 4784055401ULL, 5166779851ULL, 5580122297ULL, 6026532101ULL,
			6508654673ULL, 7029347053ULL, 7591694819ULL, 8199030421ULL, 8854952867ULL,
			9563349119ULL, 10328417069ULL, 11154690451ULL, 12047065699ULL, 13010830967ULL,
			14051697461ULL, 15175833269ULL, 16389899941ULL, 17701091957ULL, 19117179373ULL,
			20646553723ULL, 22298278033ULL, 24082140281ULL, 26008711511ULL, 28089408449ULL,
			30336561173ULL, 32763486083ULL, 35384564987ULL, 38215330193ULL, 41272556651ULL,
			44574361187ULL, 48140310083ULL, 51991534919ULL, 56150857753ULL, 60642926381ULL,
			65494360541ULL, 70733909347ULL, 76392622079ULL, 82504031879ULL, 89104354399ULL,
			96232702793ULL, 103931318963ULL, 112245824501ULL, 121225490437ULL, 130923529673ULL,
			141397412111ULL, 152709205057ULL, 164925941419ULL, 178120016741ULL, 192369618101ULL,
			207759187511ULL, 224379922513ULL, 242330316311ULL, 261716741621ULL, 282654080953ULL,
			305266407451ULL, 329687720017ULL, 356062737617ULL, 384547756627ULL, 415311577169ULL,
			448536503321ULL, 484419423629ULL, 523172977573ULL, 565026815681ULL, 610228960949ULL,
			659047277803ULL, 711771060031ULL, 768712744871ULL, 830209764437ULL, 896626545649ULL,
			968356669213ULL, 1045825202761ULL, 1129491218971ULL, 1219850516501ULL, 1317438557807ULL,
			1422833642437ULL, 1536660333827ULL, 1659593160593ULL, 1792360613377ULL, 1935749462467ULL,
			2090609419501ULL, 2257858173013ULL, 2438486826851ULL, 2633565772987ULL, 2844251034907ULL,
			3071791117649ULL, 3317534407043ULL, 3582937159577ULL, 3869572132343ULL, 4179137902963ULL,
			4513468935179ULL, 4874546449987ULL, 5264510165987ULL, 5685670979293ULL, 6140524657601ULL,
			6631766630263ULL, 7162307960627ULL, 7735292597519ULL, 8354116005307ULL, 9022445285701ULL,
			9744240908581ULL, 10523780181223ULL, 11365682595727ULL, 12274937203381ULL, 13256932179649ULL,
			14317486754059ULL, 15462885694489ULL, 16699916549977ULL, 18035909873899ULL, 19478782663799ULL,
			21037085276951ULL, 22720052099071ULL, 24537656266997ULL, 26500668768337ULL, 28620722269831ULL,
			30910380051433ULL, 33383210455559ULL, 36053867292013ULL, 38938176675341ULL, 42053230809371ULL,
			45417489274139ULL, 49050888416039ULL, 52974959489371ULL, 57212956248491ULL, 61789992748369ULL,
			66733192168217ULL, 72071847541697ULL, 77837595345043ULL, 84064602972611ULL, 90789771210419ULL,
			98052952907333ULL, 105897189139817ULL, 114368964271087ULL, 123518481412687ULL, 133399959925747ULL,
			144071956719767ULL, 155597713257341ULL, 168045530317919ULL, 181489172743357ULL, 196008306562823ULL,
			211688971087873ULL, 228624088774889ULL, 246914015876867ULL, 266667137147029ULL, 288000508118773ULL,
			311040548768281ULL, 335923792669783ULL, 362797696083323ULL, 391821511770037ULL, 423167232711623ULL,
			457020611328517ULL, 493582260234787ULL, 533068841053571ULL, 575714348337871ULL, 621771496204883ULL,
			671513215901351ULL, 725234273173393ULL, 783253015027279ULL, 845913256229507ULL, 913586316727801ULL,
			986673222066011ULL, 1065607079831353ULL, 1150855646217793ULL, 1242924097915217ULL, 1342358025748463ULL,
			1449746667808337ULL, 1565726401232969ULL, 1690984513331611ULL, 1826263274398153ULL, 1972364336349989ULL,
			2130153483258011ULL, 2300565761918687ULL, 2484611022872117ULL, 2683379904701879ULL, 2898050297078077ULL,
			3129894320844281ULL, 3380285866511837ULL, 3650708735832773ULL, 3942765434699387ULL, 4258186669475327ULL,
			4598841603033361ULL, 4966748931276059ULL, 5364088845778147ULL, 5793215953440343ULL, 6256673229715583ULL,
			6757207088092873ULL, 7297783655140267ULL, 7881606347551487ULL, 8512134855355583ULL, 9193105643784143ULL,
			9928554095286847ULL, 10722838422909689ULL, 11580665496742483ULL, 12507118736481961ULL, 13507688235400421ULL,
			14588303294232511ULL, 15755367557771069ULL, 17015796962392831ULL, 18377060719384183ULL, 19847225576934919ULL,
			21435003623089711ULL, 23149803912937001ULL, 25001788225971847ULL, 27001931284049591ULL, 29162085786773521ULL,
			31495052649715469ULL, 34014656861692667ULL, 36735829410628069ULL, 39674695763478383ULL, 42848671424556587ULL,
			46276565138521177ULL, 49978690349602787ULL, 53976985577571037ULL, 58295144423776721ULL, 62958755977678849ULL,
			67995456455893139ULL, 73435092972364709ULL, 79309900410153757ULL, 85654692442966039ULL, 92507067838403357ULL,
			99907633265475613ULL, 107900243926713703ULL, 116532263440850723ULL, 125854844516118791ULL, 135923232077408311ULL,
			146797090643601029ULL, 158540857895089139ULL, 171224126526696283ULL, 184922056648832071ULL, 199715821180738573ULL,
			215693086875197671ULL, 232948533825213499ULL, 251584416531230621ULL, 271711169853729031ULL, 293448063442027349ULL,
			316923908517389531ULL, 342277821198780751ULL, 369660046894683203ULL, 399232850646257863ULL, 431171478697958569ULL,
			465665196993795233ULL, 502918412753298863ULL, 543151885773562841ULL, 586604036635447829ULL, 633532359566283689ULL,
			684214948331586437ULL, 738952144198113299ULL, 798068315733962263ULL, 861913780992679397ULL, 930866883472093723ULL,
			1005336234149861521ULL, 1085763132881850509ULL, 1172624183512398619ULL, 1266434118193390651ULL, 1367748847648861771ULL,
			1477168755460770829ULL, 1595342255897632519ULL, 1722969636369443081ULL, 1860807207278998021ULL, 2009671783861317889ULL,
			2170445526570223663ULL, 2344081168695841793ULL, 2531607662191509001ULL, 2734136275166829739ULL, 2952867177180176387ULL,
			3189096551354590729ULL, 3444224275462959617ULL, 3719762217499996699ULL, 4017343194899996179ULL, 4338730650491996219ULL,
			4685829102531356677ULL, 5060695430733864967ULL, 5465551065192574999ULL, 5902795150407981197ULL, 6375018762440619061ULL,
			6885020263435869323ULL, 7435821884510738489ULL, 8030687635271598091ULL, 8673142646093326381ULL, 9366994057780793437ULL,
			10116353582403256369ULL, 10925661868995516457ULL, 11799714818515161101ULL, 12743692003996373039ULL, 13763187364316084263ULL,
			14864242353461370929ULL, 16053381741738279023ULL, 17337652281077342261ULL, 18446744073709551557ULL,
			
			// Sentinel
			18446744073709551557ULL
		};
		// Exclude end sentinel, thus value returned by lower_bound is always valid.
		static const size_t largePrimeCount = sizeof(largePrimes) / sizeof(largePrimes[0]) - 1U;

		const uint8 *primeCapacity = std::lower_bound(largePrimes, largePrimes + largePrimeCount, static_cast<uint8>(capacity));
		
		LEAN_ASSERT(max > largePrimeThreshold);

		while (*primeCapacity > max)
			--primeCapacity;

		LEAN_ASSERT(primeCapacity >= largePrimes);
		LEAN_ASSERT(*primeCapacity <= max);

		// Legal as max is of type size_t
		return static_cast<size_t>(*primeCapacity);
	}
}

/// Simple hash map base.
template < class Key, class Element,
	class Policy,
	class KeyValues,
	class Allocator >
class simple_hash_map_base
{
protected:
	typedef std::pair<const Key, Element> value_type_;

	typedef typename Allocator::template rebind<value_type_>::other allocator_type_;
	allocator_type_ m_allocator;

	value_type_ *m_elements;
	value_type_ *m_elementsEnd;

	typedef typename allocator_type_::size_type size_type_;
	size_type_ m_count;
	size_type_ m_capacity;

	float m_maxLoadFactor;
	
	static const size_type_ s_maxElements = static_cast<size_type_>(-1) / sizeof(value_type_);
	// Use end element to allow for proper iteration termination
	static const size_type_ s_maxBuckets = s_maxElements - 1U;
	// Keep one slot open at all times to simplify wrapped find loop termination conditions
	static const size_type_ s_maxSize = s_maxBuckets - 1U;
	static const size_type_ s_minSize = (32U < s_maxSize) ? 32U : s_maxSize;

	// Make sure size_type is unsigned
	LEAN_STATIC_ASSERT(s_maxSize > static_cast<size_type_>(0));

	/// Gets the number of buckets required from the given capacity.
	LEAN_INLINE size_type_ buckets_from_capacity(size_type_ capacity)
	{
		LEAN_ASSERT(capacity <= s_maxSize);

		float bucketHint = capacity / m_maxLoadFactor;
		
		size_type_ bucketCount = (bucketHint >= s_maxSize)
			? s_maxSize
			: static_cast<size_type_>(bucketHint);

		// Keep one slot open at all times to simplify wrapped find loop termination conditions
		return max(bucketCount, capacity) + 1U;
	}
	/// Gets the capacity from the given number of buckets.
	LEAN_INLINE size_type_ capacity_from_buckets(size_type_ buckets)
	{
		// Keep one slot open at all times to simplify wrapped find loop termination conditions
		// -> Unsigned overflow handles buckets == 0
		return min(
			static_cast<size_type_>(buckets * m_maxLoadFactor),
			buckets - 1U );
	}

	/// Checks whether the given element is physically contained by this hash map.
	LEAN_INLINE bool contains_element(const value_type_ *element) const
	{
		return (m_elements <= element) && (element < m_elementsEnd);
	}
	/// Checks whether the given element is physically contained by this hash map.
	LEAN_INLINE bool contains_element(const value_type_ &element) const
	{
		return contains_element(lean::addressof(element));
	}

	/// Marks the given element as end element.
	static LEAN_INLINE void mark_end(value_type_ *dest)
	{
		new( const_cast<void*>(static_cast<const void*>(lean::addressof(dest->first))) ) Key(KeyValues::end_key);
	}
	/// Invalidates the given element.
	static LEAN_INLINE void invalidate(value_type_ *dest)
	{
		new( const_cast<void*>(static_cast<const void*>(lean::addressof(dest->first))) ) Key(KeyValues::invalid_key);
	}
	/// Invalidates the elements in the given range.
	static void invalidate(value_type_ *dest, value_type_ *destEnd)
	{
		value_type_ *destr = dest;

		try
		{
			for (; dest != destEnd; ++dest)
				invalidate(dest);
		}
		catch (...)
		{
			destruct_keys(destr, dest);
			throw;
		}
	}
	/// Prepares the given element for actual data storage.
	static LEAN_INLINE void revalidate(value_type_ *dest)
	{
		destruct_key(dest);
	}

	/// Destructs only the key of the given element.
	static LEAN_INLINE void destruct_key(value_type_ *destr)
	{
		if (!Policy::no_key_destruct)
			destr->first.~Key();
	}
	/// Destructs the keys of the elements in the given range.
	static void destruct_keys(value_type_ *destr, value_type_ *destrEnd)
	{
		if (!Policy::no_key_destruct)
			for (; destr != destrEnd; ++destr)
				destruct_key(destr);
	}

	/// Helper class that moves element construction exception handling
	/// into a destructor, resulting in less code being generated and
	/// providing automated handling of unexpected invalidation exceptions.
	class invalidate_guard : public noncopyable
	{
	private:
		value_type_ *m_dest;
		bool m_armed;

	public:
		/// Stores an element to be invalidated on destruction, if not disarmed.
		LEAN_INLINE explicit invalidate_guard(value_type_ *dest, bool armed = true)
			: m_dest(dest),
			m_armed(armed) { }
		/// Destructs the stored element, of not disarmed.
		LEAN_INLINE ~invalidate_guard()
		{
			if (m_armed)
				invalidate(m_dest);
		}
		///  Disarms this guard.
		LEAN_INLINE void disarm() { m_armed = false; }
	};

	/// Helper class that moves element destruction exception handling
	/// into a destructor, resulting in less code being generated and
	/// providing automated handling of unexpected invalidation exceptions.
	class invalidate_n_guard : public noncopyable
	{
	private:
		value_type_ *m_dest;
		value_type_ *m_destEnd;
		bool m_armed;

	public:
		/// Stores elements to be invalidated on destruction, if not disarmed.
		LEAN_INLINE explicit invalidate_n_guard(value_type_ *dest, value_type_ *destEnd, bool armed = true)
			: m_dest(dest),
			m_destEnd(destEnd),
			m_armed(armed) { }
		/// Destructs the stored elements, of not disarmed.
		LEAN_INLINE ~invalidate_n_guard()
		{
			if (m_armed)
				invalidate(m_dest, m_destEnd);
		}
		///  Disarms this guard.
		LEAN_INLINE void disarm() { m_armed = false; }
	};

	/// Default constructs an element at the given location.
	LEAN_INLINE void default_construct(value_type_ *dest, const Key &key)
	{
		invalidate_guard guard(dest);

		revalidate(dest);
		m_allocator.construct(dest, value_type_(key, Element()));

		guard.disarm();
	}
	/// Copies the given source element to the given destination.
	LEAN_INLINE void copy_construct(value_type_ *dest, const value_type_ &source)
	{
		invalidate_guard guard(dest);

		revalidate(dest);
		m_allocator.construct(dest, source);

		guard.disarm();
	}
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move_construct(value_type_ *dest, value_type_ &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		invalidate_guard guard(dest);
		
		revalidate(dest);
		m_allocator.construct(dest, std::move(source));
		
		guard.disarm();
#else
		copy_construct(dest, source);
#endif
	}
	/// Moves the given source element to the given destination.
	LEAN_INLINE void move(value_type_ *dest, value_type_ &source)
	{
#ifndef LEAN0X_NO_RVALUE_REFERENCES
		const_cast<Key&>(dest->first) = std::move(const_cast<Key&>(source.first));
		dest->second = std::move(source.second);
#else
		const_cast<Key&>(dest->first) = const_cast<Key&>(source.first);
		dest->second = source.second;
#endif
	}
	
	/// Destructs the given VALID element.
	LEAN_INLINE void destruct_element(value_type_ *destr)
	{
		if (!Policy::no_destruct)
			m_allocator.destroy(destr);
	}

	/// Initializes the this hash map base.
	LEAN_INLINE explicit simple_hash_map_base(float maxLoadFactor)
		: m_elements(nullptr),
		m_elementsEnd(nullptr),
		m_count(0),
		m_capacity(0),
		m_maxLoadFactor(maxLoadFactor) { }
	/// Initializes the this hash map base.
	LEAN_INLINE simple_hash_map_base(float maxLoadFactor, const allocator_type_ &allocator)
		: m_allocator(allocator),
		m_elements(nullptr),
		m_elementsEnd(nullptr),
		m_count(0),
		m_capacity(0),
		m_maxLoadFactor(maxLoadFactor) { }
	/// Initializes the this hash map base.
	LEAN_INLINE simple_hash_map_base(const simple_hash_map_base &right)
		: m_allocator(right.m_allocator),
		m_elements(nullptr),
		m_elementsEnd(nullptr),
		m_count(0),
		m_capacity(0),
		m_maxLoadFactor(right.m_maxLoadFactor) { }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Initializes the this hash map base.
	LEAN_INLINE simple_hash_map_base(simple_hash_map_base &&right)
		: m_allocator(std::move(right.m_allocator)),
		m_elements(std::move(right.m_elements)),
		m_elementsEnd(std::move(right.m_elementsEnd)),
		m_count(std::move(right.m_count)),
		m_capacity(std::move(right.m_capacity)),
		m_maxLoadFactor(std::move(right.m_maxLoadFactor)) { }
#endif

	/// Does nothing.
	LEAN_INLINE simple_hash_map_base& operator =(const simple_hash_map_base&) { return *this; }
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Does nothing.
	LEAN_INLINE simple_hash_map_base& operator =(simple_hash_map_base&&) { return *this; }
#endif

	/// Swaps the contents of this hash map base and the given hash map base.
	LEAN_INLINE void swap(simple_hash_map_base &right) throw()
	{
		swap(m_allocator, right.m_allocator);
		std::swap(m_elements, right.m_elements);
		std::swap(m_elementsEnd, right.m_elementsEnd);
		std::swap(m_count, right.m_count);
		std::swap(m_capacity, right.m_capacity);
		std::swap(m_maxLoadFactor, right.m_maxLoadFactor);
	}
};

}

/// Simple and fast hash map class, partially implementing the STL hash map interface.
template < class Key, class Element,
	class Policy = simple_hash_map_policies::nonpod,
	class Hash = hash<Key>,
	class KeyValues = default_keys<Key>,
    class Pred = std::equal_to<Key>,
	class Allocator = std::allocator<Element> >
class simple_hash_map : private impl::simple_hash_map_base<Key, Element, Policy, KeyValues, Allocator>
{
private:
	typedef impl::simple_hash_map_base<Key, Element, Policy, KeyValues, Allocator> base_type;

	typedef Hash hasher_;
	hasher_ m_hasher;
	typedef Pred key_equal_;
	key_equal_ m_keyEqual;

	/// Destructs both valid and invalid elements in the given range.
	void destruct(value_type_ *destr, value_type_ *destrEnd)
	{
		if (!Policy::no_destruct || !Policy::no_key_destruct)
			for (; destr != destrEnd; ++destr)
				if (key_valid(destr->first))
					destruct_element(destr);
				else
					destruct_key(destr);
	}

	/// Allocates space for the given number of elements.
	void reallocate(size_type_ newBucketCount)
	{
		// Make prime (required for universal modulo hashing)
		// TODO: will lead to constant rehashing when reaching s_maxBuckets
		newBucketCount = impl::next_prime_capacity(newBucketCount, s_maxBuckets);

		LEAN_ASSERT(newBucketCount <= s_maxBuckets);

		// Use end element to allow for proper iteration termination
		const size_type_ newElementCount = newBucketCount + 1U;

		value_type_ *newElements = m_allocator.allocate(newElementCount);
		value_type_ *newElementsEnd = newElements + newBucketCount;

		try
		{
			// ASSERT: End element key always valid to allow for proper iteration termination
			mark_end(newElementsEnd);

			try
			{
				invalidate(newElements, newElementsEnd);
			}
			catch(...)
			{
				destruct_key(newElementsEnd);
				throw;
			}

			if (!empty())
				try
				{
					// ASSERT: One slot always remains open, automatically terminating find loops
					LEAN_ASSERT(size() < newBucketCount);

					for (value_type_ *element = m_elements; element != m_elementsEnd; ++element)
						if (key_valid(element->first))
							move_construct(
								locate_element(element->first, newElements, newElementsEnd, newBucketCount).second,
								*element );
				}
				catch(...)
				{
					destruct(newElements, newElementsEnd);
					destruct_key(newElementsEnd);
					throw;
				}
		}
		catch(...)
		{
			m_allocator.deallocate(newElements, newElementCount);
			throw;
		}
		
		value_type_ *oldElements = m_elements;
		value_type_ *oldElementsEnd = m_elementsEnd;
		const size_type_ oldBucketCount = bucket_count();
		
		m_elements = newElements;
		m_elementsEnd = newElementsEnd;
		m_capacity = capacity_from_buckets(newBucketCount);

		if (oldElements)
			free(oldElements, oldElementsEnd, oldBucketCount + 1U);
	}

	/// Frees the given elements.
	LEAN_INLINE void free(value_type_ *elements, value_type_ *elementsEnd, size_type_ elementCount)
	{
		// ASSERT: End element key always valid to allow for proper iteration termination

		// Do nothing on exception, resources leaking anyways!
		destruct(elements, elementsEnd);
		destruct_key(elementsEnd);
		m_allocator.deallocate(elements, elementCount);
	}

	/// Frees all elements.
	LEAN_INLINE void free()
	{
		if (m_elements)
			free(m_elements, m_elementsEnd, bucket_count() + 1U);
	}

	/// Gets the first element that might contain the given key.
	LEAN_INLINE value_type_* first_element(const Key &key) const
	{
		return first_element(key, m_elements, bucket_count());
	}
	/// Gets the first element that might contain the given key.
	LEAN_INLINE value_type_* first_element(const Key &key, value_type_ *elements, size_type_ bucketCount) const
	{
		return elements + m_hasher(key) % bucketCount;
	}
	/// Gets the element stored under the given key and returns false if existent, otherwise returns true and gets a fitting open element slot.
	LEAN_INLINE std::pair<bool, value_type_*> locate_element(const Key &key) const
	{
		return locate_element(key, m_elements, m_elementsEnd, bucket_count());
	}
	/// Gets the element stored under the given key and returns false if existent, otherwise returns true and gets a fitting open element slot.
	std::pair<bool, value_type_*> locate_element(const Key &key, value_type_ *elements, value_type_ *elementsEnd, size_type_ bucketCount) const
	{
		LEAN_ASSERT(key_valid(key));

		value_type_ *element = first_element(key, elements, bucketCount);
		
		while (key_valid(element->first))
		{
			if (m_keyEqual(element->first, key))
				return std::make_pair(false, element);
			
			// Wrap around
			if (++element == elementsEnd)
				element = elements;

			// ASSERT: One slot always remains open, automatically terminating this loop
		}

		return std::make_pair(true, element);
	}
	/// Gets the element stored under the given key, if existent, returns end otherwise.
	LEAN_INLINE value_type_* find_element(const Key &key) const
	{
		value_type_ *element = first_element(key);
		
		while (key_valid(element->first))
		{
			if (m_keyEqual(element->first, key))
				return element;
			
			// Wrap around
			if (++element == m_elementsEnd)
				element = m_elements;

			// ASSERT: One slot always remains open, automatically terminating this loop
		}

		return m_elementsEnd;
	}
	/// Removes the element stored at the given location.
	LEAN_INLINE void remove_element(value_type_ *element)
	{
		// If anything goes wrong, we won't be able to fix it
		terminate_guard terminateGuard;

		value_type_ *hole = element;
		--m_count;
		
		// Wrap around
		if (++element == m_elementsEnd)
			element = m_elements;

		// Find next empty position
		while (key_valid(element->first))
		{
			value_type_ *auxElement = first_element(element->first);
			
			bool tooLate = (auxElement <= hole);
			bool tooEarly = (element < auxElement);
			bool wrong = (hole <= element) ? (tooLate || tooEarly) : (tooLate && tooEarly);
			
			// Move wrongly positioned elements into hole
			if (wrong)
			{
				move(hole, *element);
				hole = element;
			}
			
			// Wrap around
			if (++element == m_elementsEnd)
				element = m_elements;

			// ASSERT: One slot always remains open, automatically terminating this loop
		}

		destruct_element(hole);
		invalidate(hole);

		terminateGuard.disarm();
	}

	/// Grows hash map storage to fit the given new count.
	LEAN_INLINE void growTo(size_type_ newCount)
	{
		reallocate(buckets_from_capacity(grow_to_capacity_hint(newCount)));
	}
	/// Grows hash map storage to fit the given additional number of elements.
	LEAN_INLINE void grow(size_type_ count)
	{
		// Mind overflow
		LEAN_ASSERT(count <= s_maxSize);
		LEAN_ASSERT(s_maxSize - count >= size());

		growTo(size() + count);
	}
	/// Grows hash map storage to fit the given new count, not inlined.
	LEAN_NOINLINE void growToHL(size_type_ newCount)
	{
		growTo(newCount);
	}
	/// Grows hash map storage to fit the given additional number of elements, not inlined.
	LEAN_NOINLINE void growHL(size_type_ count)
	{
		grow(count);
	}

public:
	/// Simple hash map iterator class.
	template <class Element>
	class basic_iterator
	{
	friend class simple_hash_map;

	private:
		Element *m_element;

		/// Allows for the automated validation of iterators on construction.
		enum search_first_valid_t
		{
			/// Allows for the automated validation of iterators on construction.
			search_first_valid
		};

		/// Constructs an iterator from the given element.
		LEAN_INLINE basic_iterator(Element *element)
			: m_element(element) { }
		/// Constructs an iterator from the given element or the next valid element, should the current element prove invalid.
		LEAN_INLINE basic_iterator(Element *element, search_first_valid_t)
			: m_element(
				(!key_valid(element->first))
					? (++basic_iterator(element)).m_element
					: element
				) { }

	public:
		/// Type of the values iterated.
		typedef Element value_type;
		/// Type of references to the values iterated.
		typedef value_type& reference;
		/// Type of pointers to the values iterated.
		typedef value_type* pointer;

		/// Gets the current element.
		LEAN_INLINE reference operator *() const
		{
			return *m_element;
		}
		/// Gets the current element.
		LEAN_INLINE pointer operator ->() const
		{
			return m_element;
		}

		/// Continues iteration.
		LEAN_INLINE basic_iterator& operator ++()
		{
			do
			{
				++m_element;
			}
			// ASSERT: End element key is always valid
			while (!key_valid(m_element->first));

			return *this;
		}
		/// Continues iteration.
		LEAN_INLINE basic_iterator operator ++(int)
		{
			return ++basic_iterator(*this);
		}

		/// Comparison operator.
		LEAN_INLINE bool operator ==(const basic_iterator &right) const
		{
			return (m_element == right.m_element);
		}
		/// Comparison operator.
		LEAN_INLINE bool operator !=(const basic_iterator &right) const
		{
			return (m_element != right.m_element);
		}
	};

	/// Construction policy used.
	typedef Policy construction_policy;

	/// Type of the allocator used by this hash map.
	typedef allocator_type_ allocator_type;
	/// Type of the size returned by this hash map.
	typedef size_type_ size_type;
	/// Type of the difference between the addresses of two elements in this hash map.
	typedef typename allocator_type::difference_type difference_type;

	/// Type of pointers to the elements contained by this hash map.
	typedef typename allocator_type::pointer pointer;
	/// Type of constant pointers to the elements contained by this hash map.
	typedef typename allocator_type::const_pointer const_pointer;
	/// Type of references to the elements contained by this hash map.
	typedef typename allocator_type::reference reference;
	/// Type of constant references to the elements contained by this hash map.
	typedef typename allocator_type::const_reference const_reference;
	/// Type of the elements contained by this hash map.
	typedef typename allocator_type::value_type value_type;
	/// Type of the keys stored by this hash map.
	typedef Key key_type;
	/// Type of the elements contained by this hash map.
	typedef Element mapped_type;

	/// Type of iterators to the elements contained by this hash map.
	typedef basic_iterator<value_type> iterator;
	/// Type of constant iterators to the elements contained by this hash map.
	typedef basic_iterator<const value_type> const_iterator;

	/// Type of the hash function.
	typedef hasher_ hasher;
	/// Type of the key comparison function.
	typedef key_equal_ key_equal;

	/// Constructs an empty hash map.
	simple_hash_map()
		: base_type(0.75f)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		reallocate(s_minSize);
	}
	/// Constructs an empty hash map.
	explicit simple_hash_map(size_type capacity, float maxLoadFactor = 0.75f)
		: base_type(maxLoadFactor)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		growTo(capacity);
	}
	/// Constructs an empty hash map.
	simple_hash_map(size_type capacity, float maxLoadFactor, const hasher& hash)
		: base_type(maxLoadFactor),
		m_hasher(hash)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		growTo(capacity);
	}
	/// Constructs an empty hash map.
	simple_hash_map(size_type capacity, float maxLoadFactor, const hasher& hash, const key_equal& keyComp)
		: base_type(maxLoadFactor),
		m_hasher(hash),
		m_keyEqual(keyComp)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		growTo(capacity);
	}
	/// Constructs an empty hash map.
	simple_hash_map(size_type capacity, float maxLoadFactor, const hasher& hash, const key_equal& keyComp, const allocator_type &allocator)
		: base_type(maxLoadFactor, allocator),
		m_hasher(hash),
		m_keyEqual(keyComp)
	{
		LEAN_ASSERT(key_valid(KeyValues::end_key));
		growTo(capacity);
	}
	/// Copies all elements from the given hash map to this hash map.
	simple_hash_map(const simple_hash_map &right)
		: base_type(right),
		m_hasher(right.m_hasher),
		m_keyEqual(right.m_keyEqual)
	{
		assign(right.begin(), right.end());
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given hash map to this hash map.
	simple_hash_map(simple_hash_map &&right)
		: base_type(std::move(right)),
		m_hasher(std::move(right.m_hasher)),
		m_keyEqual(std::move(right.m_keyEqual))
	{
		right.m_elements = nullptr;
		right.m_elementsEnd = nullptr;
		right.m_count = 0;
		right.m_capacity = 0;
	}
#endif
	/// Destroys all elements in this hash map.
	~simple_hash_map()
	{
		free();
	}

	/// Copies all elements of the given hash map to this hash map.
	simple_hash_map& operator =(const simple_hash_map &right)
	{
		if (&right != this)
			assign(right.begin(), right.end());
		return *this;
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Moves all elements from the given hash map to this hash map.
	simple_hash_map& operator =(simple_hash_map &&right)
	{
		if (&right != this)
		{
			free();

			m_elements = std::move(right.m_elements);
			m_elementsEnd = std::move(right.m_elementsEnd);

			right.m_elements = nullptr;
			right.m_elementsEnd = nullptr;

			m_allocator = std::move(right.m_allocator);
		}
		return *this;
	}
#endif

	/// Assigns the given range of elements to this hash map.
	template <class Iterator>
	void assign(Iterator source, Iterator sourceEnd)
	{
		LEAN_ASSERT(source <= sourceEnd);

		// Clear before reallocation to prevent full-range moves
		clear();

		size_type count = sourceEnd - source;

		if (count > capacity())
			growToHL(count);

		while (source != sourceEnd)
			insert(*(source++));
	}

	/// Inserts a default-constructed value into the hash map using the given key, if none
	/// stored under the given key yet, otherwise returns the one currently stored.
	LEAN_INLINE reference insert(const key_type &key)
	{
		LEAN_ASSERT(key_valid(key));

		if (m_count == capacity())
			growHL(1);

		std::pair<bool, value_type*> element = locate_element(key);
		
		if (element.first)
		{
			default_construct(element.second, key);
			++m_count;
		}
		return *element.second;
	}
	/// Inserts the given key-value-pair into this hash map.
	LEAN_INLINE std::pair<bool, iterator> insert(const value_type &value)
	{
		LEAN_ASSERT(key_valid(value.first));

		if (m_count == capacity())
		{
			if (contains_element(value))
				return std::make_pair( false, iterator(const_cast<value_type*>(lean::addressof(value))) );

			growHL(1);
		}

		std::pair<bool, value_type*> element = locate_element(value.first);

		if (element.first)
		{
			copy_construct(element.second, value);
			++m_count;
		}
		return std::make_pair(element.first, iterator(element.second));
	}
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	/// Inserts the given key-value-pair into this hash map.
	LEAN_INLINE std::pair<bool, iterator> insert(value_type &&value)
	{
		LEAN_ASSERT(key_valid(value.first));

		if (m_count == capacity())
		{
			if (contains_element(value))
				return std::make_pair( false, iterator(lean::addressof(value)) );

			growHL(1);
		}

		std::pair<bool, value_type*> element = locate_element(value.first);

		if (element.first)
		{
			move_construct(element.second, value);
			++m_count;
		}
		return std::make_pair(element.first, iterator(element.second));
	}
#endif
	/// Removes the element stored under the given key, if any.
	LEAN_INLINE size_type erase(const key_type &key)
	{
		value_type* element = find_element(key);

		if (element != m_elementsEnd)
		{
			remove_element(element);
			return 1;
		}
		else
			return 0;
	}
	/// Removes the element that the given iterator is pointing to.
	LEAN_INLINE iterator erase(iterator where)
	{
		LEAN_ASSERT(contains_element(where.m_element));
		
		remove_element(where.m_element);
		return iterator(where.m_element, iterator::search_first_valid);
	}

	/// Clears all elements from this hash map.
	LEAN_INLINE void clear()
	{
		// Elements invalidated by guard in all cases
		invalidate_n_guard invalidateGuard(m_elements, m_elementsEnd);
		m_count = 0;

		// Don't handle exceptions, memory leaking anyways
		destruct(m_elements, m_elementsEnd);
	}

	/// Reserves space for the predicted number of elements given.
	LEAN_INLINE void reserve(size_type newCapacity)
	{
		if (newCapacity > capacity())
			reallocate(buckets_from_capacity(newCapacity));
	}
	/// Tries to grow or shrink the hash map to fit the given number of elements given.
	/// The hash map will never shrink below the number of elements currently stored.
	LEAN_INLINE void rehash(size_type newCapacity)
	{
		newCapacity = max(count(), newCapacity);

		if (newCapacity != capacity())
			reallocate(buckets_from_capacity(newCapacity));
	}

	/// Gets an element by key, returning end() on failure.
	LEAN_INLINE iterator find(const key_type &key) { return iterator(find_element(key)); }
	/// Gets an element by key, returning end() on failure.
	LEAN_INLINE const_iterator find(const key_type &key) const { return const_iterator(find_element(key)); }

	/// Gets an element by key, inserts a new default-constructed one if none existent yet.
	LEAN_INLINE reference operator [](const key_type &key) { return insert(key); }

	/// Returns an iterator to the first element contained by this hash map.
	LEAN_INLINE iterator begin(void) { return iterator(m_elements, iterator::search_first_valid); }
	/// Returns a constant iterator to the first element contained by this hash map.
	LEAN_INLINE const_iterator begin(void) const { return const_iterator(m_elements, const_iterator::search_first_valid); }
	/// Returns an iterator beyond the last element contained by this hash map.
	LEAN_INLINE iterator end(void) { return iterator(m_elementsEnd); }
	/// Returns a constant iterator beyond the last element contained by this hash map.
	LEAN_INLINE const_iterator end(void) const { return const_iterator(m_elementsEnd); }

	/// Gets a copy of the allocator used by this hash map.
	LEAN_INLINE allocator_type get_allocator() const { return m_allocator; };

	/// Returns true if the given key is valid.
	LEAN_INLINE static bool key_valid(const key_type &key) { return !KeyValues::key_equal::predicate(key, KeyValues::invalid_key); }

	/// Returns true if the hash map is empty.
	LEAN_INLINE bool empty(void) const { return (m_count == 0); };
	/// Returns the number of elements contained by this hash map.
	LEAN_INLINE size_type size(void) const { return m_count; };
	/// Returns the number of elements this hash map could contain without reallocation.
	LEAN_INLINE size_type capacity(void) const { return m_capacity; };
	/// Gets the current number of buckets.
	LEAN_INLINE size_type bucket_count() const { return m_elementsEnd - m_elements; }

	/// Gets the maximum load factor.
	LEAN_INLINE float max_load_factor() const { return m_maxLoadFactor; }
	/// Sets the maximum load factor.
	void max_load_factor(float factor)
	{
		m_maxLoadFactor = factor;

		// Make sure capacity never goes below the number of elements currently stored
		// -> Capacity equal count will result in reallocation on next element insertion
		m_capacity = max(
			capacity_from_buckets(bucket_count()),
			count());
	}

	/// Computes a new capacity based on the given number of elements to be stored.
	size_type grow_to_capacity_hint(size_type count) const
	{
		size_type oldCapacity = capacity();
		LEAN_ASSERT(oldCapacity <= s_maxSize);

		// Try to double capacity (mind overflow)
		size_type newCapacity = (s_maxSize - oldCapacity < oldCapacity)
			? 0
			: oldCapacity + oldCapacity;

		// Mind overflow
		LEAN_ASSERT(count <= s_maxSize);
		
		if (newCapacity < count)
			newCapacity = count;
		
		return newCapacity;
	}

	/// Estimates the maximum number of elements that may be constructed.
	LEAN_INLINE size_type max_size() const
	{
		return s_maxSize;
	}

	/// Swaps the contents of this hash map and the given hash map.
	LEAN_INLINE void swap(simple_hash_map &right) throw()
	{
		swap(m_keyEqual, right.m_keyEqual);
		swap(m_hasher, right.m_hasher);
		base_type::swap(right);
	}
};

/// Swaps the contents of the given hash maps.
template <class Element, class Policy, class Allocator>
LEAN_INLINE void swap(simple_hash_map<Element, Policy, Allocator> &left, simple_hash_map<Element, Policy, Allocator> &right)
{
	left.swap(right);
}

} // namespace

namespace simple_hash_map_policies = containers::simple_hash_map_policies;
using containers::simple_hash_map;

} // namespace

#endif