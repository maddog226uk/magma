
/**
 * @file /magma/providers/prime/prime.h
 *
 * @brief The refactored DIME (aka PRIME) interfaces used to handle the various signet/key and message formats.
 */

#ifndef PRIME_H
#define PRIME_H

#define SECP256K1_KEY_PUB_LEN 33
#define SECP256K1_KEY_PRIV_LEN 32
#define SECP256K1_SHARED_SECRET_LEN 32

#define ED25519_KEY_PUB_LEN 32
#define ED25519_KEY_PRIV_LEN 32
#define ED25519_SIGNATURE_LEN 64

#define AES_TAG_LEN 16
#define AES_KEY_LEN 32
#define AES_BLOCK_LEN 16
#define AES_VECTOR_LEN 16

/**
 * @typedef ed25519_key_type_t
 */
typedef enum {
	ED25519_ERR,
	ED25519_PUB,
	ED25519_PRIV
} ed25519_key_type_t;

/**
 * @typedef secp256k1_key_type_t
 */
typedef enum {
	SECP256K1_ERR,
	SECP256K1_PUB,
	SECP256K1_PRIV
} secp256k1_key_type_t;

/**
 * @typedef prime_encoding_t
 */
typedef enum {
	BINARY,                               /**< Serialized object in binary form. */
	ARMORED,                              /**< An object that has been aromored using the Privacy Enhanced Message format. */
	DEBUG                                 /**< Printable version of the object, with labels, and binary data encoded using base64. */
} prime_encoding_t;

/**
 * @typedef prime_flags_t
 */
typedef enum {
	NONE,
	SECURITY                              /**< Store the object in secure memory. */
} prime_flags_t;

/**
 * @typedef prime_message_type_t
 */
typedef enum {
    PRIME_MESSAGE_ENCRYPTED = 1847,        /**< An encrypted message. */
    PRIME_MESSAGE_SENT = 1851,             /**< An encrypted, appended, sent message. */
    PRIME_MESSAGE_DRAFT = 1861,            /**< An encrypted, appended, message draft. */
    PRIME_MESSAGE_NAKED = 1908,            /**< An encrypted, imported, unstructured, naked message. */

    PRIME_MESSAGE_BOUNCE = 1931,           /**< An encapsulated, encrypted message, that has bounced. */
    PRIME_MESSAGE_FORWARD = 1948,          /**< An encapsulated, encrypted message, that has been forwarded. */
    PRIME_MESSAGE_ABUSE = 2001,            /**< An encapsulated, encrypted message, sent as an abuse complaint. */

    PRIME_BINARY_OBJECT = 1837,            /**< A binary object. */
    PRIME_PROTOCOL_TICKET = 1841           /**< An encrypted protocol ticket. */
} prime_message_type_t;

/**
 * @typedef prime_artifact_type_t
 */
typedef enum {
    PRIME_ORG_SIGNET = 1776,               /**< Organizational signet. */
	PRIME_ORG_KEY = 1952,                  /**< Organizational key. */
	PRIME_ORG_KEY_ENCRYPTED = 1947,        /**< Encrypted organizational key. */

	PRIME_USER_SIGNING_REQUEST = 1215,     /**< User signing request. */
    PRIME_USER_SIGNET = 1789,              /**< User signet. */
	PRIME_USER_KEY = 2013,                 /**< User key. */
	PRIME_USER_KEY_ENCRYPTED = 1976,       /**< Encrypted user key. */
} prime_artifact_type_t;

/**
 * @typedef prime_message_chunk_type_t
 */typedef enum {
	PRIME_CHUNK_INVALID = -1,

	// Tracing
    PRIME_CHUNK_TRACING = 0,                        /**< Tracing data. */

	// Envelope Block
	PRIME_CHUNK_EPHEMERAL = 1,                      /**< Ephemeral chunk. */
	PRIME_CHUNK_ORIGIN = 2,                         /**< Origin chunk. */
	PRIME_CHUNK_DESTINATION = 3,                    /**< Destination chunk. */

	// Metadata Block
	PRIME_CHUNK_COMMON = 32,                        /**< Common headers chunk. */
	PRIME_CHUNK_HEADERS = 33,                       /**< Remaining headers chunk. */

	// Body
	PRIME_CHUNK_BODY = 48,                          /**< Naked unstructured message body. */

	// Signature Block
	PRIME_SIGNATURE_TREE = 224,
	PRIME_SIGNATURE_USER = 225,

	PRIME_SIGNATURE_ORGIN = 254,
	PRIME_SIGNATURE_DESTINATION = 255
} prime_message_chunk_type_t;

/**
 * @typedef prime_message_chunk_flags_t
 */
typedef enum {
	PRIME_CHUNK_FLAG_INVALID = -1,

	PRIME_CHUNK_FLAG_NONE = 0,                   /**< The empty set. */

	// Envelope Block
	PRIME_CHUNK_FLAG_ALTERNATE_PADDING = 1,      /**< Alternate chunk padding algorithm. */
	PRIME_CHUNK_FLAG_ALTERNATE_ENCRYPT = 2,      /**< Payload is further encrypted by the alternate cipher suite. */
	PRIME_CHUNK_FLAG_COMPRESSED = 4,             /**< The data was compressed before being encrypted. */

	// The values 8, 16, 32, 64 are currently reserved for future use.

	PRIME_CHUNK_FLAG_SPANNING = 128              /**< The payload continues into the next available chunk. */
} prime_message_chunk_flags_t;

/**
 * @typedef prime_org_artifact_fields_t
 */
typedef enum {
	ORG_PRIMARY_KEY = 1,
	ORG_SECONDARY_KEY = 2,
	ORG_ENCRYPTION_KEY = 3,
	ORG_SELF_SIGNATURE = 4,
	ORG_FULL_SIGNATURE = 253,
	ORG_IDENTIFIER = 254,
	ORG_IDENTIFIABLE_SIGNATURE = 255
} prime_org_artifact_fields_t;

/**
 * @typedef prime_user_artifact_fields_t
 */
typedef enum {
	USER_SIGNING_KEY = 1,
	USER_ENCRYPTION_KEY = 2,
	USER_ALTERNATE_ENCRYPTION_KEY = 3,
	USER_CUSTODY_SIGNATURE = 4,
	USER_SELF_SIGNATURE = 5,
	USER_CRYPTO_SIGNATURE = 6,
	USER_FULL_SIGNATURE = 253,
	USER_IDENTIFIER = 254,
	USER_IDENTIFIABLE_SIGNATURE = 255
} prime_user_artifact_fields_t;

/**
 * @typedef prime_type_t
 */
typedef uint16_t prime_type_t;

// Allows the inclusion of this PRIME header without having to include the OpenSSL headers.
#ifdef HEADER_EC_H
typedef EC_KEY secp256k1_key_t;
#else
typedef void secp256k1_key_t;
#endif

/**
 * @typedef ed25519_key_t
 */
typedef struct __attribute__ ((packed)) {
	ed25519_key_type_t type;
	struct __attribute__ ((packed)) {
		uint8_t private[ED25519_KEY_PRIV_LEN];
		uint8_t public[ED25519_KEY_PUB_LEN];
	};
} ed25519_key_t;

/**
 * @typedef prime_user_key_t
 */
typedef struct __attribute__ ((packed)) {
	ed25519_key_t *signing;
	secp256k1_key_t *encryption;
} prime_user_key_t;

/**
 * @typedef prime_org_key_t
 */
typedef struct __attribute__ ((packed)) {
	ed25519_key_t *signing;
	secp256k1_key_t *encryption;
} prime_org_key_t;

/**
 * @typedef prime_user_signet_t
 */
typedef struct __attribute__ ((packed)) {
	ed25519_key_t *signing;                /**< User signing key, field 1. */
	secp256k1_key_t *encryption;           /**< User encryption key, field 2. */
	struct {
		stringer_t *custody;               /**< User chain of custody signature, field 4. */
		stringer_t *user;                  /**< User signature, field 5. */
		stringer_t *org;                   /**< Organizational signature, field 6. */
	} signatures;
} prime_user_signet_t;

/**
 * @typedef prime_org_signet_t
 */
typedef struct __attribute__ ((packed)) {
	ed25519_key_t *signing;                /**< Organizational signing key, field 1. */
	secp256k1_key_t *encryption;           /**< Organizational encryption key, field 3. */
	stringer_t *signature;                 /**< Organizational signature, field 4. */
} prime_org_signet_t;

/**
 * @typedef prime_chunk_keys_t
 */
typedef struct __attribute__ ((packed)) {

	ed25519_key_t *signing;
	secp256k1_key_t *encryption;

	secp256k1_key_t *author;
	secp256k1_key_t *origin;
	secp256k1_key_t *destination;
	secp256k1_key_t *recipient;

} prime_chunk_keys_t;

/**
 * @typedef prime_chunk_keks_t
 */
typedef struct __attribute__ ((packed)) {
	stringer_t *author;
	stringer_t *origin;
	stringer_t *destination;
	stringer_t *recipient;
} prime_chunk_keks_t;

/**
 * @typedef prime_chunk_slots_t
 */
typedef struct __attribute__ ((packed)) {
	placer_t author;
	placer_t origin;
	placer_t destination;
	placer_t recipient;
	placer_t buffer;
} prime_chunk_slots_t;

/**
 * @typedef prime_ephemeral_chunk_t
 */
typedef struct __attribute__ ((packed)) {
	struct {
		uint8_t type;                      /**< Chunk type, which curently, should always be 1. */
		uint32_t length;                   /**< Payload length. Currently, this should only be 35 or 69. */
	} header;
	struct {
		ed25519_key_t *signing;
		secp256k1_key_t *encryption;
	} keys;
	stringer_t *buffer;
} prime_ephemeral_chunk_t;

/**
 * @typedef prime_encrypted_chunk_t
 */
typedef struct __attribute__ ((packed)) prime_encrypted_chunk_t {
	struct {
		uint8_t type;                      /**< Chunk type, 1 through 255. */
		uint32_t length;                   /**< Payload length, must be divisible by 16 and less than 2^24 - 1. */
	} header;

	stringer_t *signature;
	uint32_t length;
	uint8_t flags;
	uint8_t pad;
	stringer_t *data;
	stringer_t *trailing;
	stringer_t *encrypted;
	prime_chunk_slots_t *slots;
	struct prime_encrypted_chunk_t *next;

} prime_encrypted_chunk_t;

/**
 * @typedef prime_signature_tree_t
 */
typedef struct __attribute__ ((packed)) {
	inx_t *tree;
} prime_signature_tree_t;

/**
 * @typedef prime_message_t
 */
typedef struct __attribute__ ((packed)) {

	prime_chunk_keys_t keys;
	prime_chunk_keks_t keks;

	struct {
		prime_ephemeral_chunk_t *ephemeral;
		prime_encrypted_chunk_t *origin;
		prime_encrypted_chunk_t *destination;
	} envelope;

	struct {
		prime_encrypted_chunk_t *common;
		prime_encrypted_chunk_t *headers;
	} metadata;

	struct {
		prime_encrypted_chunk_t *body;
	} content;

	struct {
		stringer_t *tree;
		stringer_t *user;
		stringer_t *org;
	} signatures;

	stringer_t *encrypted;

} prime_message_t;

/**
 * @typedef prime_t
 */
typedef struct __attribute__ ((packed)) {
	prime_type_t type;
	prime_flags_t flags;
	struct {
		union {
			prime_org_key_t *org;
			prime_user_key_t *user;
		};
	} key;
	struct {
		union {
			prime_org_signet_t *org;
			prime_user_signet_t *user;
		};
	} signet;
	struct {
		union {
			prime_message_t *naked;
			prime_message_t *native;
		};
	} message;
} prime_t;

// The magma org key/signet.
extern prime_t *org_key;
extern prime_t *org_signet;

/// prime.c
prime_t *     prime_alloc(prime_type_t type, prime_flags_t flags);
void          prime_cleanup(prime_t *object);
void          prime_free(prime_t *object);
stringer_t *  prime_get(prime_t *object, prime_encoding_t encoding, stringer_t *output);
prime_t *     prime_key_decrypt(stringer_t *key, stringer_t *object, prime_encoding_t encoding, prime_flags_t flags);
stringer_t *  prime_key_encrypt(stringer_t *key, prime_t *object, prime_encoding_t encoding, stringer_t *output);
prime_t *     prime_key_generate(prime_artifact_type_t type, prime_flags_t flags);
stringer_t *  prime_message_decrypt(stringer_t *message, prime_t *org, prime_t *user);
stringer_t *  prime_message_encrypt(stringer_t *message, prime_t *author, prime_t *origin, prime_t *destination, prime_t *recipient);
prime_t *     prime_request_generate(prime_t *object, prime_t *previous);
prime_t *     prime_request_sign(prime_t *request, prime_t *org);
prime_t *     prime_set(stringer_t *object, prime_encoding_t encoding, prime_flags_t flags);
stringer_t *  prime_signet_fingerprint(prime_t *object, stringer_t *output);
prime_t *     prime_signet_generate(prime_t *object);
bool_t        prime_signet_validate(prime_t *object, prime_t *validator);
bool_t        prime_start(void);
void          prime_stop(void);

#include "transposition/transposition.h"
#include "cryptography/cryptography.h"
#include "messages/messages.h"
#include "signets/signets.h"
#include "keys/keys.h"

#endif
