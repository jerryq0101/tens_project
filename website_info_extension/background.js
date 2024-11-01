chrome.runtime.onInstalled.addListener(() => {
        chrome.action.setBadgeText({
          text: "OFF",
        });
});

chrome.action.onClicked.addListener(async (tab) => {
        const prevState = await chrome.action.getBadgeText({ tabId: tab.id });
        // Next state will always be the opposite
        const nextState = prevState === 'ON' ? 'OFF' : 'ON';

        console.log("SEND DATA");
        chrome.runtime.sendNativeMessage(
                'com.jerryq0101.tens_project_communication',
                {state: nextState == "ON" ? 1 : 0},
                function (response) {
                        console.log("Received " + response.on);
                }
        );

        // Set the action badge to the next state
        await chrome.action.setBadgeText({
                text: nextState,
        });
});

chrome.webRequest.onBeforeRequest.addListener(
        function(request) {
                // Ignore requests made by service workers
                if (request.frameId === -1) {
                        return;
                }

                if ((new RegExp("instagram.com")).test(request.url))
                {
                        console.log(request.url);
                        chrome.runtime.sendNativeMessage(
                                'com.jerryq0101.tens_project_communication',
                                {state: 1},
                                function (response) {
                                        console.log("Received " + response.on);
                                }
                        );
                        state = 1;
                }
        },
        {
                urls: ["<all_urls>"]
        },
        []
);

let shockTabId = -1;
let state = 0;

chrome.webRequest.onCompleted.addListener(
        function(details)
        {
                // Ignore requests made by service workers
                if (details.frameId === -1) {
                        return;
                }

                if ((new RegExp("google.com")).test(details.url) && state) {
                        // Store the tab ID
                        shockTabId = details.tabId;
                        console.log("Tab ID stored:", shockTabId);
                        state = 0;
                }
        },
        {
                urls: ["<all_urls>"]
        },
        []
);

chrome.tabs.onRemoved.addListener(
        function(tabId, removeInfo) {
                if (tabId === shockTabId)
                {
                        console.log("Tab Closed:", tabId);
                        // Send native message to stop the shock
                        chrome.runtime.sendNativeMessage(
                                'com.jerryq0101.tens_project_communication',
                                { state: 0 },
                                function(response) {
                                console.log("Received " + response.on);
                                }
                        );
                }
        }
);
