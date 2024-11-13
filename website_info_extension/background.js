chrome.runtime.onInstalled.addListener(() => {
        chrome.action.setBadgeText({
          text: "OFF",
        });
});


let shockTabId = -1;
let state = 0;

chrome.webRequest.onBeforeRequest.addListener(
        async function(request) {
                // Ignore requests made by service workers
                if (request.frameId === -1) {
                        return;
                }
                
                // Blocking instagram
                if ((new RegExp("instagram.com")).test(request.url))
                {
                        // Debug to check its the right url
                        console.log(request.url);
                        
                        // Send message to activate shock
                        chrome.runtime.sendNativeMessage(
                                'com.jerryq0101.tens_project_communication',
                                {state: 1},
                                function (response) {
                                        console.log("Received " + response.on);
                                }
                        );
                        
                        // Set state to allow this specific shock redirect to be stored
                        state = 1;
                }
        },
        {
                urls: ["<all_urls>"]
        },
        []
);

chrome.webRequest.onCompleted.addListener(
        async function(details)
        {
                // Ignore requests made by service workers (since even after closing tabs they make requests and shit)
                if (details.frameId === -1) {
                        return;
                }

                // Procrastination specific URL and Tab has completed loading and 
                if ((new RegExp("google.com")).test(details.url) && state) 
                        {
                        
                        shockTabId = details.tabId;
                        console.log("Tab ID stored:", shockTabId);
                        state = 0;

                        // Change the badge state
                        const nextState = 'ON';
                        await chrome.action.setBadgeText({
                                text: nextState,
                        });
                }
        },
        {
                urls: ["<all_urls>"]
        },
        []
);

chrome.tabs.onRemoved.addListener(
        async function(tabId, removeInfo) {
                if (tabId === shockTabId) // If we closed the procrastination tab
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

                        // Change the badge state
                        const nextState = 'OFF';
                        await chrome.action.setBadgeText({
                                text: nextState,
                        });
                }
        }
);
