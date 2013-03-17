#include "GameServer.h"
#include <WinSock2.h>
#include "MemStream.h"
#include "PacketUtils.h"
#include <assert.h>
#include "Character.h"
#include "PathUtils.h"
#include "StdStreamUtils.h"
#include "GameServer_Login.h"
#include "GameServer_MoveOutOfRoom.h"

#define LOGNAME		("GameServer")

static PacketData GetMotd()
{
	std::vector<const char*> messages;
	messages.push_back("Welcome to the Seventh Umbral Server");
	messages.push_back("Compiled on: " __DATE__);
	messages.push_back("---------------");

	const static uint8 packetHeader[] =
	{
		0x01, 0x00, 0x00, 0x00, 0x58, 0x02, 0x06, 0x00, 0xF9, 0x18, 0x8B, 0xEE, 0x3B, 0x01, 0x00, 0x00
	};

	PacketData outgoingPacket;
	outgoingPacket.insert(std::end(outgoingPacket), std::begin(packetHeader), std::end(packetHeader));

	for(auto message : messages)
	{
		uint8 messagePacket[0x248] = 
		{
			0x48, 0x02, 0x03, 0x00, 0x41, 0x29, 0x9B, 0x02, 0x41, 0x29, 0x9B, 0x02, 0x00, 0xE0, 0xD2, 0xFE, 
			0x14, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0xE8, 0xE0, 0x50, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x1D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};

		memcpy(messagePacket + 0x44, message, strlen(message) + 1);

		outgoingPacket.insert(std::end(outgoingPacket), std::begin(messagePacket), std::end(messagePacket));
	}

	uint32 packetSize = outgoingPacket.size();
	assert(packetSize < 0x10000);

	*reinterpret_cast<uint16*>(outgoingPacket.data() + 0x4) = static_cast<uint16>(packetSize);

	return outgoingPacket;
}

static PacketData GetCharacterInfo()
{
	PacketData outgoingPacket(std::begin(g_client0_login8), std::end(g_client0_login8));

	CCharacter character;
	auto personalDataPath = Framework::PathUtils::GetPersonalDataPath();
	auto characterPath = personalDataPath / "ffxivd_character.xml";
	if(boost::filesystem::exists(characterPath))
	{
		auto inputStream = Framework::CreateInputStdStream(characterPath.native());
		character.Load(inputStream);
	}

	const uint32 characterInfoBase = 0x368;

	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x20]) = CCharacter::GetModelFromTribe(character.tribe);
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x28]) = character.size;
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x30]) = character.GetColorInfo();
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x38]) = character.GetFaceInfo();
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x40]) = character.hairStyle << 10;
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x48]) = character.voice;
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x50]) = 0;						//weapon

	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x88]) = character.headGear;		//headGear
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x90]) = character.bodyGear;		//bodyGear
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0x98]) = character.legsGear;		//legsGear
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0xA0]) = character.handsGear;	//handsGear
	*reinterpret_cast<uint32*>(&outgoingPacket[characterInfoBase + 0xA8]) = character.feetGear;		//feetGear

	//Insert character name
	for(unsigned int i = 0; i < character.name.size(); i++)
	{
		outgoingPacket[characterInfoBase + 0x14C + i] = character.name[i];
	}
	outgoingPacket[characterInfoBase + 0x14C + character.name.size()] = 0;

	return outgoingPacket;
}

static uint16 GetSubPacketCommand(const PacketData& subPacket)
{
	return *reinterpret_cast<const uint16*>(subPacket.data() + 0x12);
}

static void SendPacket(SOCKET clientSocket, const void* packet, size_t size)
{
	int sent = send(clientSocket, reinterpret_cast<const char*>(packet), size, 0);
	assert(sent == size);
}

static void UpdateEntities(SOCKET clientSocket)
{
	static int entityUpdateCounter = 0;

	if(entityUpdateCounter >= 5)
	{
		entityUpdateCounter = 0;

		uint8 entityStatusPacket[] =
		{
			0x01, 0x00, 0x00, 0x00, 0x60, 0x00, 0x1D, 0x00, 0x75, 0x1C, 0x8B, 0xEE, 0x3B, 0x01, 0x00, 0x00,
			0x50, 0x00, 0x03, 0x00, 0x06, 0x00, 0xa0, 0x47, 0x41, 0x29, 0x9b, 0x02, 0x00, 0xe0, 0xd2, 0xfe,
			0x14, 0x00, 0xcf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x60, 0xe1, 0x50, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xd7, 0x1f, 0x43, 0x33, 0x33, 0x33, 0x3f,
			0x85, 0x2b, 0x27, 0x43, 0xcd, 0xcc, 0x4c, 0xbe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		};

		float itemPosX = *reinterpret_cast<float*>(&entityStatusPacket[0x38]);
		float itemPosY = *reinterpret_cast<float*>(&entityStatusPacket[0x3C]);
		float itemPosZ = *reinterpret_cast<float*>(&entityStatusPacket[0x40]);

		static float someAlpha = 0;
		itemPosX += 5.f * sin(someAlpha);
		itemPosZ += 5.f * cos(someAlpha);
		someAlpha += 0.1f;

		*reinterpret_cast<float*>(&entityStatusPacket[0x38]) = itemPosX;
		*reinterpret_cast<float*>(&entityStatusPacket[0x3C]) = itemPosY;
		*reinterpret_cast<float*>(&entityStatusPacket[0x40]) = itemPosZ;


		//Send entity status
		{
			std::vector<uint8> outgoingPacket(std::begin(entityStatusPacket), std::end(entityStatusPacket));
			int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
			assert(sent == outgoingPacket.size());
		}
	}

	entityUpdateCounter++;
}

static void ClientThreadProc(SOCKET clientSocket, int clientId)
{
	u_long notBlockingMode = 1;
	ioctlsocket(clientSocket, FIONBIO, &notBlockingMode);
	Framework::CMemStream incomingStream;

	printf("%s: Received connection.\r\n", LOGNAME);

	uint32 moorPart2Pending = 0;
	uint32 moorPart3Pending = 0;
	uint32 moorPart4Pending = 0;
	uint32 moorPart5Pending = 0;
	bool alreadyMovedOutOfRoom = false;

	while(1)
	{
		if(moorPart2Pending != 0)
		{
			Sleep(100);

			moorPart2Pending--;
			if(moorPart2Pending == 0)
			{
				SendPacket(clientSocket, g_client0_moor20, sizeof(g_client0_moor20));
				SendPacket(clientSocket, g_client0_moor21, sizeof(g_client0_moor21));
				SendPacket(clientSocket, g_client0_moor22, sizeof(g_client0_moor22));
				SendPacket(clientSocket, g_client0_moor23, sizeof(g_client0_moor23));
				SendPacket(clientSocket, g_client0_moor24, sizeof(g_client0_moor24));

				moorPart3Pending = 10;
			}
		}

		if(moorPart3Pending != 0)
		{
			Sleep(100);

			moorPart3Pending--;
			if(moorPart3Pending == 0)
			{
				SendPacket(clientSocket, g_client0_moor25, sizeof(g_client0_moor25));
				SendPacket(clientSocket, g_client0_moor26, sizeof(g_client0_moor26));
				SendPacket(clientSocket, g_client0_moor27, sizeof(g_client0_moor27));
				SendPacket(clientSocket, g_client0_moor28, sizeof(g_client0_moor28));
				SendPacket(clientSocket, g_client0_moor29, sizeof(g_client0_moor29));

				moorPart4Pending = 10;
			}
		}

		if(moorPart4Pending != 0)
		{
			Sleep(100);

			moorPart4Pending--;
			if(moorPart4Pending == 0)
			{
				SendPacket(clientSocket, g_client0_moor30, sizeof(g_client0_moor30));
				SendPacket(clientSocket, g_client0_moor31, sizeof(g_client0_moor31));
				SendPacket(clientSocket, g_client0_moor32, sizeof(g_client0_moor32));
				SendPacket(clientSocket, g_client0_moor33, sizeof(g_client0_moor33));
				SendPacket(clientSocket, g_client0_moor34, sizeof(g_client0_moor34));

				moorPart5Pending = 10;
			}
		}

		if(moorPart5Pending != 0)
		{
			Sleep(100);

			moorPart5Pending--;
			if(moorPart5Pending == 0)
			{
				SendPacket(clientSocket, g_client0_moor35, sizeof(g_client0_moor35));
				SendPacket(clientSocket, g_client0_moor36, sizeof(g_client0_moor36));
				SendPacket(clientSocket, g_client0_moor37, sizeof(g_client0_moor37));
				SendPacket(clientSocket, g_client0_moor38, sizeof(g_client0_moor38));
				SendPacket(clientSocket, g_client0_moor39, sizeof(g_client0_moor39));
				SendPacket(clientSocket, g_client0_moor40, sizeof(g_client0_moor40));
			}
		}

		//Read from socket
		{
			static const unsigned int maxPacketSize = 0x10000;
			uint8 incomingPacket[maxPacketSize];
			int read = recv(clientSocket, reinterpret_cast<char*>(incomingPacket), maxPacketSize, 0);
			if(read == 0)
			{
				//Client disconnected
				printf("%s: Client disconnected.\r\n", LOGNAME);
				break;
			}
			if(read > 0)
			{
				incomingStream.Write(incomingPacket, read);
			}
		}
		if(CPacketUtils::HasPacket(incomingStream))
		{
			auto incomingPacket = CPacketUtils::ReadPacket(incomingStream);
			auto subPackets = CPacketUtils::SplitPacket(incomingPacket);
			bool killConnection = false;

			for(const auto& subPacket : subPackets)
			{
				if((clientId == 0) && (subPacket.size() == 0x38) && (subPacket[0x10] == 0x00))
				{
					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login1), std::end(g_client0_login1));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login2), std::end(g_client0_login2));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login3), std::end(g_client0_login3));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login4), std::end(g_client0_login4));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						auto outgoingPacket = GetMotd();
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login7), std::end(g_client0_login7));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						auto outgoingPacket = GetCharacterInfo();
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login9), std::end(g_client0_login9));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login10), std::end(g_client0_login10));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login11), std::end(g_client0_login11));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login12), std::end(g_client0_login12));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login13), std::end(g_client0_login13));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client0_login14), std::end(g_client0_login14));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}
				}
				else if((clientId == 0) && (subPacket.size() == 0x38) && (subPacket[0x12] == 0x01))
				{
					//Some keep alive thing? (only time is updated here)
					uint32 clientTime = *reinterpret_cast<const uint32*>(&subPacket[0x18]);
					uint32 moreTime = *reinterpret_cast<const uint32*>(&subPacket[0x20]);

					uint8 keepAlivePacket[0x50] =
					{
						0x01, 0x00, 0x00, 0x00, 0x50, 0x00, 0x01, 0x00, 0xEF, 0xCB, 0xA4, 0xEE, 0x3B, 0x01, 0x00, 0x00,
						0x40, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9b, 0x02, 0x41, 0x29, 0x9b, 0x02, 0x00, 0xe0, 0xd2, 0xfe,
						0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcb, 0xee, 0xe0, 0x50, 0x00, 0x00, 0x00, 0x00,
						0x4a, 0x18, 0x9c, 0x0a, 0x4d, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
					};

					*reinterpret_cast<uint32*>(&keepAlivePacket[0x28]) = clientTime;
					*reinterpret_cast<uint32*>(&keepAlivePacket[0x30]) = moreTime;

					{
						std::vector<uint8> outgoingPacket(std::begin(keepAlivePacket), std::end(keepAlivePacket));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}
				}
				else if((clientId == 0) && (subPacket.size() == 0x40) && (GetSubPacketCommand(subPacket) == 0xCA))
				{
					//Some keep alive thing?
					uint32 clientTime = *reinterpret_cast<const uint32*>(&subPacket[0x18]);
					float posX = *reinterpret_cast<const float*>(&subPacket[0x28]);
					float posY = *reinterpret_cast<const float*>(&subPacket[0x2C]);
					float posZ = *reinterpret_cast<const float*>(&subPacket[0x30]);

//					printf("%s: Client Id (%d): Keeping Alive. Time: 0x%0.8X, Pos: (X: %f, Y: %f, Z: %f).\r\n",
//						LOGNAME, clientId, clientTime, posX, posY, posZ);
				}
				else if((clientId == 0) && (subPacket.size() == 0xD8) && (GetSubPacketCommand(subPacket) == 0x12D))
				{
					//commandRequest (emote, changing equipment, ...)

					uint32 clientTime = *reinterpret_cast<const uint32*>(&subPacket[0x18]);
					uint32 commandId = *reinterpret_cast<const uint32*>(&subPacket[0x20]);
					const char* commandName = reinterpret_cast<const char*>(subPacket.data()) + 0x31;

					printf("%s: Client Id (%d): %s Id = 0x%0.8X.\r\n", LOGNAME, clientId, commandName, commandId);

					if(!strcmp(commandName, "commandRequest"))
					{
						uint8 emoteId = subPacket[0x55];

						printf("%s: Executing Emote 0x%0.2X\r\n", LOGNAME, emoteId);

						uint8 commandRequestPacket[0x40] =
						{
							0x01, 0x00, 0x00, 0x00, 0x40, 0x00, 0x01, 0x00, 0x52, 0xE2, 0xA4, 0xEE, 0x3B, 0x01, 0x00, 0x00,
							0x30, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9b, 0x02, 0x41, 0x29, 0x9b, 0x02, 0x00, 0xe0, 0xd2, 0xfe,
							0x14, 0x00, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd1, 0xee, 0xe0, 0x50, 0x00, 0x00, 0x00, 0x00,
							0x00, 0xb0, 0x00, 0x05, 0x41, 0x29, 0x9b, 0x02, 0x6e, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						};

						//In: 0x6F, Out: (0x0500B000, 0x526E) -> Dance
						//In: 0x??, Out: (0x5000C000, 0x????) -> Angry Pointing
						//In: 0x??, Out: (0x5000D000, 0x????) -> Snooze
						//In: 0x??, Out: (0x5000E000, 0x????) -> Frustrated
						//In: 0x??, Out: (0x5000F000, 0x????) -> Military Sign
						//In: 0x??, Out: (0x50011000, 0x????) -> Shrug
						//In: 0x??, Out: (0x50012000, 0x????) -> Success Baby
						//In: 0x77, Out: (0x05013000, 0x52BE) -> Kneel
						//In: 0x??, Out: (0x50014000, 0x????) -> Chuckle
						//In: 0x??, Out: (0x50015000, 0x????) -> Laugh
						//In: 0x??, Out: (0x50016000, 0x????) -> Look
						//In: 0x??, Out: (0x50018000, 0x????) -> No
						//In: 0x??, Out: (0x50019000, 0x????) -> Never
					
						uint32 animationId = 0x0500B000;
						uint32 descriptionId = 0x526E;

						//Wrong emotes
						//gcsalute		-> grovel
						//grovel		-> serpent salute
						//blowkiss		-> disappointed
						//pray			-> firedance
						//airquote		-> pray
						//pose			-> blowkiss
						//happy			-> maelstorm salute
						//disappointed	-> pose

						if(emoteId >= 0x64 && emoteId < 0xA0)
						{
							animationId = 0x05000000 + ((emoteId - 0x64) << 12);
						}
/*
						switch(emoteId)
						{
						case 0x6A:		//Cheer
							animationId = 0x05006000;
							break;
						case 0x6F:		//Dance
							animationId = 0x0500B000;
							break;
						case 0x71:		//Doze
							animationId = 0x0500D000;
							break;
						case 0x75:		//Huh
							animationId = 0x05011000;
							break;
						case 0x78:		//Chuckle
							animationId = 0x05014000;
							break;
						case 0x79:		//Laugh
							animationId = 0x05015000;
							break;
						}
*/

						*reinterpret_cast<uint32*>(&commandRequestPacket[0x28]) = clientTime;
						*reinterpret_cast<uint32*>(&commandRequestPacket[0x30]) = animationId;
						*reinterpret_cast<uint32*>(&commandRequestPacket[0x38]) = descriptionId;

//						printf("Anim Id = 0x%0.8X, Desc Id = 0x%0.8X\r\n", animationId, descriptionId);
//						animationId += 0x1000;
//						descriptionId += 1;

						{
							std::vector<uint8> outgoingPacket(std::begin(commandRequestPacket), std::end(commandRequestPacket));
							int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
							assert(sent == outgoingPacket.size());
						}
					}
					else if(!strcmp(commandName, "talkDefault"))
					{
						uint8 commandRequestPacket[] =
						{
							0x01, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x01, 0x00, 0x52, 0xE2, 0xA4, 0xEE, 0x3B, 0x01, 0x00, 0x00,
							0xb0, 0x00, 0x03, 0x00, 0x41, 0x29, 0x9b, 0x02, 0x41, 0x29, 0x9b, 0x02, 0x00, 0xe0, 0xd2, 0xfe,
							0x14, 0x00, 0x30, 0x01, 0x00, 0x00, 0x00, 0x00, 0xd3, 0xe9, 0xe0, 0x50, 0x00, 0x00, 0x00, 0x00,
							0x41, 0x29, 0x9b, 0x02, 0x07, 0x00, 0xa0, 0x47, 0x01, 0x74, 0x61, 0x6c, 0x6b, 0x44, 0x65, 0x66,
							0x61, 0x75, 0x6c, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x65, 0x6c, 0x65, 0x67, 0x61, 0x74,
							0x65, 0x45, 0x76, 0x65, 0x6e, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x02, 0x9b, 0x29, 0x41, 0x06, 0xa0,
							0xf1, 0xaf, 0xcd, 0x02, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x54, 0x61, 0x6c, 0x6b, 0x57,
							0x69, 0x74, 0x68, 0x49, 0x6e, 0x6e, 0x5f, 0x45, 0x78, 0x69, 0x74, 0x44, 0x6f, 0x6f, 0x72, 0x00,
							0x05, 0x05, 0x05, 0x05, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0xe8, 0x4e, 0x40, 0x00, 0x00, 0x00,
						};

						{
							std::vector<uint8> outgoingPacket(std::begin(commandRequestPacket), std::end(commandRequestPacket));
							int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
							assert(sent == outgoingPacket.size());
						}
					}
					else
					{
						//Anything else will probably crash, so just bail
						killConnection = true;
						break;
					}
				}
				else if((clientId == 0) && (subPacket.size() == 0x78) && (GetSubPacketCommand(subPacket) == 0x12E))
				{
					if(!alreadyMovedOutOfRoom)
					{
						printf("%s: Command 0x12E: Moving out of room\r\n", LOGNAME);

						SendPacket(clientSocket, g_client0_moor1, sizeof(g_client0_moor1));
						SendPacket(clientSocket, g_client0_moor2, sizeof(g_client0_moor2));
						SendPacket(clientSocket, g_client0_moor3, sizeof(g_client0_moor3));
						SendPacket(clientSocket, g_client0_moor4, sizeof(g_client0_moor4));
						SendPacket(clientSocket, g_client0_moor5, sizeof(g_client0_moor5));
						SendPacket(clientSocket, g_client0_moor6, sizeof(g_client0_moor6));
						SendPacket(clientSocket, g_client0_moor7, sizeof(g_client0_moor7));
						SendPacket(clientSocket, g_client0_moor8, sizeof(g_client0_moor8));
						SendPacket(clientSocket, g_client0_moor9, sizeof(g_client0_moor9));
						SendPacket(clientSocket, g_client0_moor10, sizeof(g_client0_moor10));
						SendPacket(clientSocket, g_client0_moor11, sizeof(g_client0_moor11));
						SendPacket(clientSocket, g_client0_moor12, sizeof(g_client0_moor12));
						SendPacket(clientSocket, g_client0_moor13, sizeof(g_client0_moor13));
						SendPacket(clientSocket, g_client0_moor14, sizeof(g_client0_moor14));
						SendPacket(clientSocket, g_client0_moor15, sizeof(g_client0_moor15));
						SendPacket(clientSocket, g_client0_moor16, sizeof(g_client0_moor16));
						SendPacket(clientSocket, g_client0_moor17, sizeof(g_client0_moor17));
						SendPacket(clientSocket, g_client0_moor18, sizeof(g_client0_moor18));
						SendPacket(clientSocket, g_client0_moor19, sizeof(g_client0_moor19));

						alreadyMovedOutOfRoom = true;
						moorPart2Pending = 10;
					}
				}
				else if((clientId == 1) && (subPacket.size() == 0x38) && (subPacket[0x10] == 0x00))
				{
					{
						std::vector<uint8> outgoingPacket(std::begin(g_client1_login1), std::end(g_client1_login1));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}

					{
						std::vector<uint8> outgoingPacket(std::begin(g_client1_login2), std::end(g_client1_login2));
						int sent = send(clientSocket, reinterpret_cast<const char*>(outgoingPacket.data()), outgoingPacket.size(), 0);
						assert(sent == outgoingPacket.size());
					}
				}
				else
				{
					printf("%s: Client Id (%d): Received unknown packet of size 0x%0.4X.\r\n%s\r\n",
						LOGNAME, clientId, subPacket.size(), CPacketUtils::DumpPacket(subPacket).c_str());
				}
			}

			if(killConnection)
			{
				break;
			}
		}
	}

	closesocket(clientSocket);
}

CGameServer::CGameServer()
{

}

CGameServer::~CGameServer()
{

}

void CGameServer::Start()
{
	std::thread serverThread(std::bind(&CGameServer::ServerThreadProc, this));
	m_serverThread = std::move(serverThread);
}

void CGameServer::ServerThreadProc()
{
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in service;
	service.sin_family			= AF_INET;
	service.sin_addr.s_addr		= inet_addr("127.0.0.1");
	service.sin_port			= htons(GAME_SERVER_PORT);
	if(bind(listenSocket, reinterpret_cast<sockaddr*>(&service), sizeof(sockaddr_in)))
	{
		printf("Failed to bind socket.\r\n");
		return;
	}

	if(listen(listenSocket, SOMAXCONN))
	{
		printf("Failed to listen on socket.\r\n");
		return;
	}

	printf("Game server started.\r\n");

	int clientId = 0;

	while(1)
	{
		sockaddr_in incomingAddr;
		int incomingAddrSize = sizeof(sockaddr_in);
		SOCKET incomingSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&incomingAddr), &incomingAddrSize);
		std::thread clientThread(std::bind(&ClientThreadProc, incomingSocket, clientId & 1));
		clientThread.detach();
		clientId++;
	}

	closesocket(listenSocket);
}
