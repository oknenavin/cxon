// Copyright (c) 2017-2022 oknenavin.
// Licensed under the MIT license. See LICENSE file in the library root for full license information.
//
// SPDX-License-Identifier: MIT

#ifndef CXON_JSON_TEST_NATIVE_HXX_
#define CXON_JSON_TEST_NATIVE_HXX_

#include "cxon/json.hxx"

#include "cxon/lib/node.ordered.hxx"
#include "cxon/lib/std/map.hxx"
#include "cxon/lib/std/optional.hxx"
#include "cxon/lib/std/string.hxx"
#include "cxon/lib/std/utility.hxx"
#include "cxon/lib/std/vector.hxx"
#include "cxon/lib/std/deque.hxx"

namespace test {

    namespace blns {

        using object = std::vector<std::string>;

    }

    namespace emoji {

        struct entry {
            std::string emoji;
            std::string description;
            std::string category;
            std::vector<std::string> aliases;
            std::vector<std::string> tags;
            std::string unicode_version;
            std::string ios_version;

            CXON_JSON_CLS_MEMBER(entry,
                CXON_JSON_CLS_FIELD_ASIS_DFLT(emoji             , self.emoji.empty()),
                CXON_JSON_CLS_FIELD_ASIS_DFLT(description       , self.description.empty()),
                CXON_JSON_CLS_FIELD_ASIS_DFLT(category          , self.category.empty()),
                CXON_JSON_CLS_FIELD_ASIS(aliases),
                CXON_JSON_CLS_FIELD_ASIS(tags),
                CXON_JSON_CLS_FIELD_ASIS_DFLT(unicode_version   , self.emoji.empty()),
                CXON_JSON_CLS_FIELD_ASIS_DFLT(ios_version       , self.ios_version.empty())
            )
        };

        using object = std::vector<entry>;

    }

    namespace apache_builds {

        struct job {
            std::string name;
            std::string url;
            std::string color;

            CXON_JSON_CLS_MEMBER(job,
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(url),
                CXON_JSON_CLS_FIELD_ASIS(color)
            )
        };

        struct view {
            std::string name;
            std::string url;

            CXON_JSON_CLS_MEMBER(view,
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(url)
            )
        };

        struct object {
            std::vector<std::map<std::string, std::string>> assignedLabels;
            std::string mode;
            std::string nodeDescription;
            std::string nodeName;
            unsigned numExecutors;
            std::string description;
            std::vector<job> jobs;
            std::map<std::string, std::string> overallLoad;
            std::map<std::string, std::string> primaryView;
            bool quietingDown;
            unsigned slaveAgentPort;
            std::map<std::string, std::string> unlabeledLoad;
            bool useCrumbs;
            bool useSecurity;
            std::vector<view> views;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(assignedLabels),
                CXON_JSON_CLS_FIELD_ASIS(mode),
                CXON_JSON_CLS_FIELD_ASIS(nodeDescription),
                CXON_JSON_CLS_FIELD_ASIS(nodeName),
                CXON_JSON_CLS_FIELD_ASIS(numExecutors),
                CXON_JSON_CLS_FIELD_ASIS(description),
                CXON_JSON_CLS_FIELD_ASIS(jobs),
                CXON_JSON_CLS_FIELD_ASIS(overallLoad),
                CXON_JSON_CLS_FIELD_ASIS(primaryView),
                CXON_JSON_CLS_FIELD_ASIS(quietingDown),
                CXON_JSON_CLS_FIELD_ASIS(slaveAgentPort),
                CXON_JSON_CLS_FIELD_ASIS(unlabeledLoad),
                CXON_JSON_CLS_FIELD_ASIS(useCrumbs),
                CXON_JSON_CLS_FIELD_ASIS(useSecurity),
                CXON_JSON_CLS_FIELD_ASIS(views)
            )
        };

    }

    namespace canada {

        struct geometry {
            std::string type;
            std::vector<std::vector<std::pair<double, double>>> coordinates;

            CXON_JSON_CLS_MEMBER(geometry,
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(coordinates)
            )
        };

        struct feature {
            std::string type;
            std::map<std::string, std::string> properties;
            struct geometry geometry;

            CXON_JSON_CLS_MEMBER(feature,
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(properties),
                CXON_JSON_CLS_FIELD_ASIS(geometry)
            )
        };

        struct object {
            std::string type;
            std::vector<feature> features;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(features)
            )
        };

    }

#   ifdef CXON_HAS_LIB_STD_OPTIONAL
        namespace citm_catalog {

            struct event {
                std::optional<std::string> description;
                unsigned long long id;
                std::optional<std::string> logo;
                std::optional<std::string> name;
                std::vector<unsigned long long> subTopicIds;
                std::optional<std::string> subjectCode;
                std::optional<std::string> subtitle;
                std::vector<unsigned long long> topicIds;

                CXON_JSON_CLS_MEMBER(event,
                    CXON_JSON_CLS_FIELD_ASIS(description),
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(logo),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(subTopicIds),
                    CXON_JSON_CLS_FIELD_ASIS(subjectCode),
                    CXON_JSON_CLS_FIELD_ASIS(subtitle),
                    CXON_JSON_CLS_FIELD_ASIS(topicIds)
                )
            };

            struct price {
                unsigned long long amount;
                unsigned long long audienceSubCategoryId;
                unsigned long long seatCategoryId;

                CXON_JSON_CLS_MEMBER(price,
                    CXON_JSON_CLS_FIELD_ASIS(amount),
                    CXON_JSON_CLS_FIELD_ASIS(audienceSubCategoryId),
                    CXON_JSON_CLS_FIELD_ASIS(seatCategoryId)
                )
            };

            struct area {
                unsigned long long areaId;
                std::vector<unsigned long long> blockIds;

                CXON_JSON_CLS_MEMBER(area,
                    CXON_JSON_CLS_FIELD_ASIS(areaId),
                    CXON_JSON_CLS_FIELD_ASIS(blockIds)
                )
            };

            struct seat {
                std::vector<area> areas;
                unsigned long long seatCategoryId;

                CXON_JSON_CLS_MEMBER(seat,
                    CXON_JSON_CLS_FIELD_ASIS(areas),
                    CXON_JSON_CLS_FIELD_ASIS(seatCategoryId)
                )
            };

            struct performance {
                unsigned long long eventId;
                unsigned long long id;
                std::optional<std::string> logo;
                std::optional<std::string> name;
                std::vector<price> prices;
                std::vector<seat> seatCategories;
                std::optional<std::string> seatMapImage;
                unsigned long long start;
                std::string venueCode;

                CXON_JSON_CLS_MEMBER(performance,
                    CXON_JSON_CLS_FIELD_ASIS(eventId),
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(logo),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(prices),
                    CXON_JSON_CLS_FIELD_ASIS(seatCategories),
                    CXON_JSON_CLS_FIELD_ASIS(seatMapImage),
                    CXON_JSON_CLS_FIELD_ASIS(start),
                    CXON_JSON_CLS_FIELD_ASIS(venueCode)
                )
            };

            struct object {
                cxon::ordered::object<std::string, std::string> areaNames;
                cxon::ordered::object<std::string, std::string> audienceSubCategoryNames;
                cxon::ordered::object<std::string, std::string> blockNames;
                cxon::ordered::object<std::string, event> events;
                std::vector<performance> performances;
                cxon::ordered::object<std::string, std::string> seatCategoryNames;
                cxon::ordered::object<std::string, std::string> subTopicNames;
                cxon::ordered::object<std::string, std::string> subjectNames;
                cxon::ordered::object<std::string, std::string> topicNames;
                cxon::ordered::object<std::string, std::vector<unsigned long long>> topicSubTopics;
                cxon::ordered::object<std::string, std::string> venueNames;

                CXON_JSON_CLS_MEMBER(object,
                    CXON_JSON_CLS_FIELD_ASIS(areaNames),
                    CXON_JSON_CLS_FIELD_ASIS(audienceSubCategoryNames),
                    CXON_JSON_CLS_FIELD_ASIS(blockNames),
                    CXON_JSON_CLS_FIELD_ASIS(events),
                    CXON_JSON_CLS_FIELD_ASIS(performances),
                    CXON_JSON_CLS_FIELD_ASIS(seatCategoryNames),
                    CXON_JSON_CLS_FIELD_ASIS(subTopicNames),
                    CXON_JSON_CLS_FIELD_ASIS(subjectNames),
                    CXON_JSON_CLS_FIELD_ASIS(topicNames),
                    CXON_JSON_CLS_FIELD_ASIS(topicSubTopics),
                    CXON_JSON_CLS_FIELD_ASIS(venueNames)
                )
            };

        }
#   endif

#   ifdef CXON_HAS_LIB_STD_OPTIONAL
        namespace github_events {

            struct actor {
                std::string gravatar_id;
                std::string login;
                std::string avatar_url;
                std::string url;
                unsigned long long id;

                CXON_JSON_CLS_MEMBER(actor,
                    CXON_JSON_CLS_FIELD_ASIS(gravatar_id),
                    CXON_JSON_CLS_FIELD_ASIS(login),
                    CXON_JSON_CLS_FIELD_ASIS(avatar_url),
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(id)
                )
            };

            struct repo {
                std::string url;
                unsigned long long id;
                std::string name;

                CXON_JSON_CLS_MEMBER(repo,
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(name)
                )
            };

            struct author {
                std::string email;
                std::string name;

                CXON_JSON_CLS_MEMBER(author,
                    CXON_JSON_CLS_FIELD_ASIS(email),
                    CXON_JSON_CLS_FIELD_ASIS(name)
                )
            };

            struct commit {
                std::string url;
                std::string message;
                bool distinct;
                std::string sha;
                struct author author;

                CXON_JSON_CLS_MEMBER(commit,
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(message),
                    CXON_JSON_CLS_FIELD_ASIS(distinct),
                    CXON_JSON_CLS_FIELD_ASIS(sha),
                    CXON_JSON_CLS_FIELD_ASIS(author)
                )
            };

            struct user {
                std::string url;
                std::string gists_url;
                std::string gravatar_id;
                std::string type;
                std::string avatar_url;
                std::string subscriptions_url;
                std::string organizations_url;
                std::string received_events_url;
                std::string repos_url;
                std::string login;
                unsigned long long id;
                std::string starred_url;
                std::string events_url;
                std::string followers_url;
                std::string following_url;

                CXON_JSON_CLS_MEMBER(user,
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(gists_url),
                    CXON_JSON_CLS_FIELD_ASIS(gravatar_id),
                    CXON_JSON_CLS_FIELD_ASIS(type),
                    CXON_JSON_CLS_FIELD_ASIS(avatar_url),
                    CXON_JSON_CLS_FIELD_ASIS(subscriptions_url),
                    CXON_JSON_CLS_FIELD_ASIS(organizations_url),
                    CXON_JSON_CLS_FIELD_ASIS(received_events_url),
                    CXON_JSON_CLS_FIELD_ASIS(repos_url),
                    CXON_JSON_CLS_FIELD_ASIS(login),
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(starred_url),
                    CXON_JSON_CLS_FIELD_ASIS(events_url),
                    CXON_JSON_CLS_FIELD_ASIS(followers_url),
                    CXON_JSON_CLS_FIELD_ASIS(following_url)
                )
            };

            struct forkee {
                std::string description;
                bool fork;
                std::string url;
                std::string language;
                std::string stargazers_url;
                std::string clone_url;
                std::string tags_url;
                std::string full_name;
                std::string merges_url;
                unsigned forks;
                bool private_;
                std::string git_refs_url;
                std::string archive_url;
                std::string collaborators_url;
                struct user owner;
                std::string languages_url;
                std::string trees_url;
                std::string labels_url;
                std::string html_url;
                std::string pushed_at;
                std::string created_at;
                bool has_issues;
                std::string forks_url;
                std::string branches_url;
                std::string commits_url;
                std::string notifications_url;
                unsigned open_issues;
                std::string contents_url;
                std::string blobs_url;
                std::string issues_url;
                std::string compare_url;
                std::string issue_events_url;
                std::string name;
                std::string updated_at;
                std::string statuses_url;
                unsigned forks_count;
                std::string assignees_url;
                std::string ssh_url;
                bool public_;
                bool has_wiki;
                std::string subscribers_url;
                std::optional<std::string> mirror_url;
                unsigned watchers_count;
                unsigned long long id;
                bool has_downloads;
                std::string git_commits_url;
                std::string downloads_url;
                std::string pulls_url;
                std::optional<std::string> homepage;
                std::string issue_comment_url;
                std::string hooks_url;
                std::string subscription_url;
                std::string milestones_url;
                std::string svn_url;
                std::string events_url;
                std::string git_tags_url;
                std::string teams_url;
                std::string comments_url;
                unsigned open_issues_count;
                std::string keys_url;
                std::string git_url;
                std::string contributors_url;
                unsigned size = 0;
                unsigned watchers;

                CXON_JSON_CLS_MEMBER(forkee,
                    CXON_JSON_CLS_FIELD_ASIS(description),
                    CXON_JSON_CLS_FIELD_ASIS(fork),
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(language),
                    CXON_JSON_CLS_FIELD_ASIS(stargazers_url),
                    CXON_JSON_CLS_FIELD_ASIS(clone_url),
                    CXON_JSON_CLS_FIELD_ASIS(tags_url),
                    CXON_JSON_CLS_FIELD_ASIS(full_name),
                    CXON_JSON_CLS_FIELD_ASIS(merges_url),
                    CXON_JSON_CLS_FIELD_ASIS(forks),
                    CXON_JSON_CLS_FIELD_NAME("private", private_),
                    CXON_JSON_CLS_FIELD_ASIS(git_refs_url),
                    CXON_JSON_CLS_FIELD_ASIS(archive_url),
                    CXON_JSON_CLS_FIELD_ASIS(collaborators_url),
                    CXON_JSON_CLS_FIELD_ASIS(owner),
                    CXON_JSON_CLS_FIELD_ASIS(languages_url),
                    CXON_JSON_CLS_FIELD_ASIS(trees_url),
                    CXON_JSON_CLS_FIELD_ASIS(labels_url),
                    CXON_JSON_CLS_FIELD_ASIS(html_url),
                    CXON_JSON_CLS_FIELD_ASIS(pushed_at),
                    CXON_JSON_CLS_FIELD_ASIS(created_at),
                    CXON_JSON_CLS_FIELD_ASIS(has_issues),
                    CXON_JSON_CLS_FIELD_ASIS(forks_url),
                    CXON_JSON_CLS_FIELD_ASIS(branches_url),
                    CXON_JSON_CLS_FIELD_ASIS(commits_url),
                    CXON_JSON_CLS_FIELD_ASIS(notifications_url),
                    CXON_JSON_CLS_FIELD_ASIS(open_issues),
                    CXON_JSON_CLS_FIELD_ASIS(contents_url),
                    CXON_JSON_CLS_FIELD_ASIS(blobs_url),
                    CXON_JSON_CLS_FIELD_ASIS(issues_url),
                    CXON_JSON_CLS_FIELD_ASIS(compare_url),
                    CXON_JSON_CLS_FIELD_ASIS(issue_events_url),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(updated_at),
                    CXON_JSON_CLS_FIELD_ASIS(statuses_url),
                    CXON_JSON_CLS_FIELD_ASIS(forks_count),
                    CXON_JSON_CLS_FIELD_ASIS(assignees_url),
                    CXON_JSON_CLS_FIELD_ASIS(ssh_url),
                    CXON_JSON_CLS_FIELD_NAME("public", public_),
                    CXON_JSON_CLS_FIELD_ASIS(has_wiki),
                    CXON_JSON_CLS_FIELD_ASIS(subscribers_url),
                    CXON_JSON_CLS_FIELD_ASIS(mirror_url),
                    CXON_JSON_CLS_FIELD_ASIS(watchers_count),
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(has_downloads),
                    CXON_JSON_CLS_FIELD_ASIS(git_commits_url),
                    CXON_JSON_CLS_FIELD_ASIS(downloads_url),
                    CXON_JSON_CLS_FIELD_ASIS(pulls_url),
                    CXON_JSON_CLS_FIELD_ASIS(homepage),
                    CXON_JSON_CLS_FIELD_ASIS(issue_comment_url),
                    CXON_JSON_CLS_FIELD_ASIS(hooks_url),
                    CXON_JSON_CLS_FIELD_ASIS(subscription_url),
                    CXON_JSON_CLS_FIELD_ASIS(milestones_url),
                    CXON_JSON_CLS_FIELD_ASIS(svn_url),
                    CXON_JSON_CLS_FIELD_ASIS(events_url),
                    CXON_JSON_CLS_FIELD_ASIS(git_tags_url),
                    CXON_JSON_CLS_FIELD_ASIS(teams_url),
                    CXON_JSON_CLS_FIELD_ASIS(comments_url),
                    CXON_JSON_CLS_FIELD_ASIS(open_issues_count),
                    CXON_JSON_CLS_FIELD_ASIS(keys_url),
                    CXON_JSON_CLS_FIELD_ASIS(git_url),
                    CXON_JSON_CLS_FIELD_ASIS(contributors_url),
                    CXON_JSON_CLS_FIELD_ASIS(size),
                    CXON_JSON_CLS_FIELD_ASIS(watchers)
                )
            };

            struct pull_request {
                std::optional<std::string> html_url;
                std::optional<std::string> patch_url;
                std::optional<std::string> diff_url;

                CXON_JSON_CLS_MEMBER(pull_request,
                    CXON_JSON_CLS_FIELD_ASIS(html_url),
                    CXON_JSON_CLS_FIELD_ASIS(patch_url),
                    CXON_JSON_CLS_FIELD_ASIS(diff_url)
                )
            };

            struct issue {
                struct user user;
                std::string url;
                std::vector<std::string> labels;
                std::string html_url;
                std::string labels_url;
                struct pull_request pull_request;
                std::string created_at;
                std::optional<std::string> closed_at;
                std::optional<std::string> milestone;
                std::string title;
                std::string body;
                std::string updated_at;
                unsigned number;
                std::string state;
                std::optional<struct user> assignee;
                unsigned long long id;
                std::string events_url;
                std::string comments_url;
                unsigned comments;

                CXON_JSON_CLS_MEMBER(issue,
                    CXON_JSON_CLS_FIELD_ASIS(user),
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(labels),
                    CXON_JSON_CLS_FIELD_ASIS(html_url),
                    CXON_JSON_CLS_FIELD_ASIS(labels_url),
                    CXON_JSON_CLS_FIELD_ASIS(pull_request),
                    CXON_JSON_CLS_FIELD_ASIS(created_at),
                    CXON_JSON_CLS_FIELD_ASIS(closed_at),
                    CXON_JSON_CLS_FIELD_ASIS(milestone),
                    CXON_JSON_CLS_FIELD_ASIS(title),
                    CXON_JSON_CLS_FIELD_ASIS(body),
                    CXON_JSON_CLS_FIELD_ASIS(updated_at),
                    CXON_JSON_CLS_FIELD_ASIS(number),
                    CXON_JSON_CLS_FIELD_ASIS(state),
                    CXON_JSON_CLS_FIELD_ASIS(assignee),
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(events_url),
                    CXON_JSON_CLS_FIELD_ASIS(comments_url),
                    CXON_JSON_CLS_FIELD_ASIS(comments)
                )
            };

            struct comment {
                struct user user;
                std::string url;
                std::string issue_url;
                std::string created_at;
                std::string body;
                std::string updated_at;
                unsigned long long id;

                CXON_JSON_CLS_MEMBER(comment,
                    CXON_JSON_CLS_FIELD_ASIS(user),
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(issue_url),
                    CXON_JSON_CLS_FIELD_ASIS(created_at),
                    CXON_JSON_CLS_FIELD_ASIS(body),
                    CXON_JSON_CLS_FIELD_ASIS(updated_at),
                    CXON_JSON_CLS_FIELD_ASIS(id)
                )
            };

            struct page {
                std::string page_name;
                std::string html_url;
                std::string title;
                std::string sha;
                std::optional<std::string> summary;
                std::string action;

                CXON_JSON_CLS_MEMBER(page,
                    CXON_JSON_CLS_FIELD_ASIS(page_name),
                    CXON_JSON_CLS_FIELD_ASIS(html_url),
                    CXON_JSON_CLS_FIELD_ASIS(title),
                    CXON_JSON_CLS_FIELD_ASIS(sha),
                    CXON_JSON_CLS_FIELD_ASIS(summary),
                    CXON_JSON_CLS_FIELD_ASIS(action)
                )
            };

            struct payload {
                // PushEvent
                std::vector<commit> commits;
                unsigned distinct_size;
                std::optional<std::string> ref;
                unsigned long long push_id;
                std::string head;
                std::string before;
                unsigned size = 0;
                // CreateEvent
                std::string description;
                std::string master_branch;
                //std::string ref;
                std::string ref_type;
                // ForkEvent
                struct forkee forkee;
                // WatchEvent
                std::string action;
                // IssueCommentEvent
                struct issue issue;
                struct comment comment;
                // IssuesEvent
                // GollumEvent
                std::vector<page> pages;
                // CreateEvent

                CXON_JSON_CLS_MEMBER(payload,
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(commits       , self.commits.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(distinct_size , self.commits.empty()),
                    CXON_JSON_CLS_FIELD_ASIS(ref),
                    //CXON_JSON_CLS_FIELD_ASIS_DFLT(ref           , self.forkee.size == 0 && self.action.empty() && self.issue.created_at.empty() && self.pages.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(push_id       , self.commits.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(head          , self.commits.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(before        , self.commits.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(size          , self.size == 0),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(description   , self.description.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(master_branch , self.master_branch.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(ref_type      , self.ref_type.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(forkee        , self.forkee.size == 0),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(action        , self.action.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(issue         , self.issue.created_at.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(comment       , self.issue.created_at.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(pages         , self.pages.empty())
                )
            };

            struct org {
                std::string gravatar_id;
                std::string login;
                std::string avatar_url;
                std::string url;
                unsigned long long id;

                CXON_JSON_CLS_MEMBER(org,
                    CXON_JSON_CLS_FIELD_ASIS(gravatar_id),
                    CXON_JSON_CLS_FIELD_ASIS(login),
                    CXON_JSON_CLS_FIELD_ASIS(avatar_url),
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(id)
                )
            };

            struct event {
                std::string type;
                std::string created_at;
                struct actor actor;
                struct repo repo;
                bool public_;
                struct payload payload;
                struct org org;
                std::string id;

                CXON_JSON_CLS_MEMBER(event,
                    CXON_JSON_CLS_FIELD_ASIS(type),
                    CXON_JSON_CLS_FIELD_ASIS(created_at),
                    CXON_JSON_CLS_FIELD_ASIS(actor),
                    CXON_JSON_CLS_FIELD_ASIS(repo),
                    CXON_JSON_CLS_FIELD_NAME("public", public_),
                    CXON_JSON_CLS_FIELD_ASIS(payload),
                    CXON_JSON_CLS_FIELD_ASIS(org),
                    CXON_JSON_CLS_FIELD_ASIS(id)
                )
            };

            using object = std::vector<event>;

        }
#   endif

    namespace gsoc_2018 {

        struct sponsor {
            std::string type;
            std::string name;
            std::string disambiguatingDescription;
            std::string description;
            std::string url;
            std::string logo;

            CXON_JSON_CLS_MEMBER(sponsor,
                CXON_JSON_CLS_FIELD_NAME("@type", type),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(disambiguatingDescription),
                CXON_JSON_CLS_FIELD_ASIS(description),
                CXON_JSON_CLS_FIELD_ASIS(url),
                CXON_JSON_CLS_FIELD_ASIS(logo)
            )
        };

        struct author {
            std::string type;
            std::string name;

            CXON_JSON_CLS_MEMBER(author,
                CXON_JSON_CLS_FIELD_NAME("@type", type),
                CXON_JSON_CLS_FIELD_ASIS(name)
            )
        };

        struct participant {
            std::string context;
            std::string type;
            std::string name;
            std::string description;
            struct sponsor sponsor;
            struct author author;

            CXON_JSON_CLS_MEMBER(participant,
                CXON_JSON_CLS_FIELD_NAME("@context", context),
                CXON_JSON_CLS_FIELD_NAME("@type", type),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(description),
                CXON_JSON_CLS_FIELD_ASIS(sponsor),
                CXON_JSON_CLS_FIELD_ASIS(author)
            )
        };

        using object = cxon::ordered::object<std::string, participant>;

    }

#   ifdef CXON_HAS_LIB_STD_OPTIONAL
        namespace instruments {

            struct envelope_node {
                unsigned tick;
                unsigned value;

                CXON_JSON_CLS_MEMBER(envelope_node,
                    CXON_JSON_CLS_FIELD_ASIS(tick),
                    CXON_JSON_CLS_FIELD_ASIS(value)
                )
            };

            struct envelope {
                unsigned loop_end;
                unsigned loop_start;
                std::vector<envelope_node> nodes;
                unsigned release_node;
                unsigned sustain_end;
                unsigned sustain_start;

                CXON_JSON_CLS_MEMBER(envelope,
                    CXON_JSON_CLS_FIELD_ASIS(loop_end),
                    CXON_JSON_CLS_FIELD_ASIS(loop_start),
                    CXON_JSON_CLS_FIELD_ASIS(nodes),
                    CXON_JSON_CLS_FIELD_ASIS(release_node),
                    CXON_JSON_CLS_FIELD_ASIS(sustain_end),
                    CXON_JSON_CLS_FIELD_ASIS(sustain_start)
                )
            };

            struct instrument {
                unsigned default_filter_cutoff;
                bool default_filter_cutoff_enabled;
                unsigned default_filter_mode;
                unsigned default_filter_resonance;
                bool default_filter_resonance_enabled;
                unsigned default_pan;
                unsigned duplicate_check_type;
                unsigned duplicate_note_action;
                unsigned fadeout;
                unsigned global_volume;
                unsigned graph_insert;
                std::string legacy_filename;
                unsigned midi_bank;
                unsigned midi_channel;
                unsigned midi_drum_set;
                unsigned midi_program;
                std::string name;
                unsigned new_note_action;
                std::nullptr_t note_map;
                envelope panning_envelope;
                envelope pitch_envelope;
                unsigned pitch_pan_center;
                unsigned pitch_pan_separation;
                unsigned pitch_to_tempo_lock;
                unsigned random_cutoff_weight;
                unsigned random_pan_weight;
                unsigned random_resonance_weight;
                unsigned random_volume_weight;
                std::nullptr_t sample_map;
                std::nullptr_t tuning;
                envelope volume_envelope;
                unsigned volume_ramp_down;
                unsigned volume_ramp_up;

                CXON_JSON_CLS_MEMBER(instrument,
                    CXON_JSON_CLS_FIELD_ASIS(default_filter_cutoff),
                    CXON_JSON_CLS_FIELD_ASIS(default_filter_cutoff_enabled),
                    CXON_JSON_CLS_FIELD_ASIS(default_filter_mode),
                    CXON_JSON_CLS_FIELD_ASIS(default_filter_resonance),
                    CXON_JSON_CLS_FIELD_ASIS(default_filter_resonance_enabled),
                    CXON_JSON_CLS_FIELD_ASIS(default_pan),
                    CXON_JSON_CLS_FIELD_ASIS(duplicate_check_type),
                    CXON_JSON_CLS_FIELD_ASIS(duplicate_note_action),
                    CXON_JSON_CLS_FIELD_ASIS(fadeout),
                    CXON_JSON_CLS_FIELD_ASIS(global_volume),
                    CXON_JSON_CLS_FIELD_ASIS(graph_insert),
                    CXON_JSON_CLS_FIELD_ASIS(legacy_filename),
                    CXON_JSON_CLS_FIELD_ASIS(midi_bank),
                    CXON_JSON_CLS_FIELD_ASIS(midi_channel),
                    CXON_JSON_CLS_FIELD_ASIS(midi_drum_set),
                    CXON_JSON_CLS_FIELD_ASIS(midi_program),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(new_note_action),
                    CXON_JSON_CLS_FIELD_ASIS(note_map),
                    CXON_JSON_CLS_FIELD_ASIS(panning_envelope),
                    CXON_JSON_CLS_FIELD_ASIS(pitch_envelope),
                    CXON_JSON_CLS_FIELD_ASIS(pitch_pan_center),
                    CXON_JSON_CLS_FIELD_ASIS(pitch_pan_separation),
                    CXON_JSON_CLS_FIELD_ASIS(pitch_to_tempo_lock),
                    CXON_JSON_CLS_FIELD_ASIS(random_cutoff_weight),
                    CXON_JSON_CLS_FIELD_ASIS(random_pan_weight),
                    CXON_JSON_CLS_FIELD_ASIS(random_resonance_weight),
                    CXON_JSON_CLS_FIELD_ASIS(random_volume_weight),
                    CXON_JSON_CLS_FIELD_ASIS(sample_map),
                    CXON_JSON_CLS_FIELD_ASIS(tuning),
                    CXON_JSON_CLS_FIELD_ASIS(volume_envelope),
                    CXON_JSON_CLS_FIELD_ASIS(volume_ramp_down),
                    CXON_JSON_CLS_FIELD_ASIS(volume_ramp_up)
                )
            };

            struct pattern_data {
                unsigned channel;
                unsigned fxcmd;
                unsigned fxparam;
                unsigned instr;
                unsigned note;
                unsigned row;
                unsigned volcmd;
                unsigned volval;

                CXON_JSON_CLS_MEMBER(pattern_data,
                    CXON_JSON_CLS_FIELD_ASIS(channel),
                    CXON_JSON_CLS_FIELD_ASIS(fxcmd),
                    CXON_JSON_CLS_FIELD_ASIS(fxparam),
                    CXON_JSON_CLS_FIELD_ASIS(instr),
                    CXON_JSON_CLS_FIELD_ASIS(note),
                    CXON_JSON_CLS_FIELD_ASIS(row),
                    CXON_JSON_CLS_FIELD_ASIS(volcmd),
                    CXON_JSON_CLS_FIELD_ASIS(volval)
                )
            };

            struct pattern {
                std::optional<std::vector<pattern_data>> data;
                std::string name;
                unsigned rows;
                unsigned rows_per_beat;
                unsigned rows_per_measure;

                CXON_JSON_CLS_MEMBER(pattern,
                    CXON_JSON_CLS_FIELD_ASIS(data),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(rows),
                    CXON_JSON_CLS_FIELD_ASIS(rows_per_beat),
                    CXON_JSON_CLS_FIELD_ASIS(rows_per_measure)
                )
            };

            struct sample {
                unsigned c5_samplerate;
                unsigned global_volume;
                std::string legacy_filename;
                unsigned length;
                unsigned loop_end;
                unsigned loop_start;
                std::string name;
                unsigned pan;
                unsigned sustain_end;
                unsigned sustain_start;
                unsigned vibrato_depth;
                unsigned vibrato_rate;
                unsigned vibrato_sweep;
                unsigned vibrato_type;
                unsigned volume;

                CXON_JSON_CLS_MEMBER(sample,
                    CXON_JSON_CLS_FIELD_ASIS(c5_samplerate),
                    CXON_JSON_CLS_FIELD_ASIS(global_volume),
                    CXON_JSON_CLS_FIELD_ASIS(legacy_filename),
                    CXON_JSON_CLS_FIELD_ASIS(length),
                    CXON_JSON_CLS_FIELD_ASIS(loop_end),
                    CXON_JSON_CLS_FIELD_ASIS(loop_start),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(pan),
                    CXON_JSON_CLS_FIELD_ASIS(sustain_end),
                    CXON_JSON_CLS_FIELD_ASIS(sustain_start),
                    CXON_JSON_CLS_FIELD_ASIS(vibrato_depth),
                    CXON_JSON_CLS_FIELD_ASIS(vibrato_rate),
                    CXON_JSON_CLS_FIELD_ASIS(vibrato_sweep),
                    CXON_JSON_CLS_FIELD_ASIS(vibrato_type),
                    CXON_JSON_CLS_FIELD_ASIS(volume)
                )
            };

            struct object {
                std::nullptr_t graphstate;
                std::vector<instrument> instruments;
                std::optional<std::string> message;
                std::string name;
                std::nullptr_t orderlist;
                std::vector<pattern> patterns;
                std::nullptr_t pluginstate;
                std::vector<sample> samples;
                unsigned short version;

                CXON_JSON_CLS_MEMBER(object,
                    CXON_JSON_CLS_FIELD_ASIS(graphstate),
                    CXON_JSON_CLS_FIELD_ASIS(instruments),
                    CXON_JSON_CLS_FIELD_ASIS(message),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(orderlist),
                    CXON_JSON_CLS_FIELD_ASIS(patterns),
                    CXON_JSON_CLS_FIELD_ASIS(pluginstate),
                    CXON_JSON_CLS_FIELD_ASIS(samples),
                    CXON_JSON_CLS_FIELD_ASIS(version)
                )
            };

        }
#   endif

    namespace marine_ik {

        struct image {
            std::string url;
            std::string uuid;
            std::string name;

            CXON_JSON_CLS_MEMBER(image,
                CXON_JSON_CLS_FIELD_ASIS(url),
                CXON_JSON_CLS_FIELD_ASIS(uuid),
                CXON_JSON_CLS_FIELD_ASIS(name)
            )
        };

        struct key {
            std::vector<float> pos;
            float time;
            std::vector<float> scl;
            std::vector<float> rot;

            CXON_JSON_CLS_MEMBER(key,
                CXON_JSON_CLS_FIELD_ASIS(pos),
                CXON_JSON_CLS_FIELD_ASIS(time),
                CXON_JSON_CLS_FIELD_ASIS(scl),
                CXON_JSON_CLS_FIELD_ASIS(rot)
            )
        };

        struct element {
            int parent;
            std::vector<key> keys;

            CXON_JSON_CLS_MEMBER(element,
                CXON_JSON_CLS_FIELD_ASIS(parent),
                CXON_JSON_CLS_FIELD_ASIS(keys)
            )
        };

        struct data_metadata {
            unsigned uvs;
            unsigned version;
            unsigned faces;
            std::string generator;
            unsigned normals;
            unsigned bones;
            unsigned vertices;

            CXON_JSON_CLS_MEMBER(data_metadata,
                CXON_JSON_CLS_FIELD_ASIS(uvs),
                CXON_JSON_CLS_FIELD_ASIS(version),
                CXON_JSON_CLS_FIELD_ASIS(faces),
                CXON_JSON_CLS_FIELD_ASIS(generator),
                CXON_JSON_CLS_FIELD_ASIS(normals),
                CXON_JSON_CLS_FIELD_ASIS(bones),
                CXON_JSON_CLS_FIELD_ASIS(vertices)
            )
        };

        struct frame {
            std::vector<element> hierarchy;
            float length;
            unsigned fps;
            std::string name;

            CXON_JSON_CLS_MEMBER(frame,
                CXON_JSON_CLS_FIELD_ASIS(hierarchy),
                CXON_JSON_CLS_FIELD_ASIS(length),
                CXON_JSON_CLS_FIELD_ASIS(fps),
                CXON_JSON_CLS_FIELD_ASIS(name)
            )
        };

        struct bone {
            int parent;
            std::vector<float> pos;
            std::vector<float> rotq;
            std::vector<float> scl;
            std::string name;

            CXON_JSON_CLS_MEMBER(bone,
                CXON_JSON_CLS_FIELD_ASIS(parent),
                CXON_JSON_CLS_FIELD_ASIS(pos),
                CXON_JSON_CLS_FIELD_ASIS(rotq),
                CXON_JSON_CLS_FIELD_ASIS(scl),
                CXON_JSON_CLS_FIELD_ASIS(name)
            )
        };

        struct data {
            std::vector<std::vector<float>> uvs;
            std::vector<frame> animations;
            std::vector<float> vertices;
            struct data_metadata metadata;
            std::string name;
            std::vector<float> skinWeights;
            std::vector<unsigned> skinIndices;
            unsigned influencesPerVertex;
            std::vector<float> normals;
            std::vector<bone> bones;
            std::vector<unsigned> faces;

            CXON_JSON_CLS_MEMBER(data,
                CXON_JSON_CLS_FIELD_ASIS(uvs),
                CXON_JSON_CLS_FIELD_ASIS(animations),
                CXON_JSON_CLS_FIELD_ASIS(vertices),
                CXON_JSON_CLS_FIELD_ASIS(metadata),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(skinWeights),
                CXON_JSON_CLS_FIELD_ASIS(skinIndices),
                CXON_JSON_CLS_FIELD_ASIS(influencesPerVertex),
                CXON_JSON_CLS_FIELD_ASIS(normals),
                CXON_JSON_CLS_FIELD_ASIS(bones),
                CXON_JSON_CLS_FIELD_ASIS(faces)
            )
        };


        struct geometry {
            std::string type;
            std::string uuid;
            struct data data;

            CXON_JSON_CLS_MEMBER(geometry,
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(uuid),
                CXON_JSON_CLS_FIELD_ASIS(data)
            )
        };

        struct texture {
            std::vector<int> repeat;
            std::vector<int> wrap;
            unsigned anisotropy;
            std::string image;
            std::string name;
            unsigned mapping;
            unsigned minFilter;
            std::string uuid;
            unsigned magFilter;

            CXON_JSON_CLS_MEMBER(texture,
                CXON_JSON_CLS_FIELD_ASIS(repeat),
                CXON_JSON_CLS_FIELD_ASIS(wrap),
                CXON_JSON_CLS_FIELD_ASIS(anisotropy),
                CXON_JSON_CLS_FIELD_ASIS(image),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(mapping),
                CXON_JSON_CLS_FIELD_ASIS(minFilter),
                CXON_JSON_CLS_FIELD_ASIS(uuid),
                CXON_JSON_CLS_FIELD_ASIS(magFilter)
            )
        };

        struct object_metadata {
            std::string sourceFile;
            std::string generator;
            std::string type;
            float version;

            CXON_JSON_CLS_MEMBER(object_metadata,
                CXON_JSON_CLS_FIELD_ASIS(sourceFile),
                CXON_JSON_CLS_FIELD_ASIS(generator),
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(version)
            )
        };

        struct material {
            unsigned vertexColors;
            std::string name;
            std::string type;
            std::string uuid;
            std::string blending;
            std::string map;
            bool transparent;
            bool depthTest;
            unsigned color;
            unsigned shininess;
            unsigned emissive;
            bool depthWrite;
            unsigned specular;

            CXON_JSON_CLS_MEMBER(material,
                CXON_JSON_CLS_FIELD_ASIS(vertexColors),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(uuid),
                CXON_JSON_CLS_FIELD_ASIS(blending),
                CXON_JSON_CLS_FIELD_ASIS(map),
                CXON_JSON_CLS_FIELD_ASIS(transparent),
                CXON_JSON_CLS_FIELD_ASIS(depthTest),
                CXON_JSON_CLS_FIELD_ASIS(color),
                CXON_JSON_CLS_FIELD_ASIS(shininess),
                CXON_JSON_CLS_FIELD_ASIS(emissive),
                CXON_JSON_CLS_FIELD_ASIS(depthWrite),
                CXON_JSON_CLS_FIELD_ASIS(specular)
            )
        };

        struct child {
            std::string name;
            std::string uuid;
            std::vector<int> matrix;
            bool visible;
            std::string type;
            std::string material;
            bool castShadow;
            bool receiveShadow;
            std::string geometry;

            CXON_JSON_CLS_MEMBER(child,
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(uuid),
                CXON_JSON_CLS_FIELD_ASIS(matrix),
                CXON_JSON_CLS_FIELD_ASIS(visible),
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(material),
                CXON_JSON_CLS_FIELD_ASIS(castShadow),
                CXON_JSON_CLS_FIELD_ASIS(receiveShadow),
                CXON_JSON_CLS_FIELD_ASIS(geometry)
            )
        };

        struct object_ {
            std::vector<child> children;
            std::string type;
            std::vector<int> matrix;
            std::string uuid;

            CXON_JSON_CLS_MEMBER(object_,
                CXON_JSON_CLS_FIELD_ASIS(children),
                CXON_JSON_CLS_FIELD_ASIS(type),
                CXON_JSON_CLS_FIELD_ASIS(matrix),
                CXON_JSON_CLS_FIELD_ASIS(uuid)
            )
        };

        struct animation {
            std::vector<int> tracks;
            unsigned fps;
            std::string name;

            CXON_JSON_CLS_MEMBER(animation,
                CXON_JSON_CLS_FIELD_ASIS(tracks),
                CXON_JSON_CLS_FIELD_ASIS(fps),
                CXON_JSON_CLS_FIELD_ASIS(name)
            )
        };

        struct object {
            std::vector<image> images;
            std::vector<geometry> geometries;
            std::vector<texture> textures;
            struct object_metadata metadata;
            std::vector<material> materials;
            struct object_ object_;
            std::vector<animation> animations;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(images),
                CXON_JSON_CLS_FIELD_ASIS(geometries),
                CXON_JSON_CLS_FIELD_ASIS(textures),
                CXON_JSON_CLS_FIELD_ASIS(metadata),
                CXON_JSON_CLS_FIELD_ASIS(materials),
                CXON_JSON_CLS_FIELD_NAME("object", object_),
                CXON_JSON_CLS_FIELD_ASIS(animations)
            )
        };

    }

    namespace mesh {

        struct batch {
            std::pair<unsigned, unsigned> indexRange;
            std::pair<unsigned, unsigned> vertexRange;
            std::vector<unsigned> usedBones;

            CXON_JSON_CLS_MEMBER(batch,
                CXON_JSON_CLS_FIELD_ASIS(indexRange),
                CXON_JSON_CLS_FIELD_ASIS(vertexRange),
                CXON_JSON_CLS_FIELD_ASIS(usedBones)
            )
        };

        struct morph_targets {
            bool dummy; // avoid compiler warning
                        // TODO: fix the macros to handle empty structs
            CXON_JSON_CLS_MEMBER(morph_targets,
                CXON_JSON_CLS_FIELD_ASIS(dummy)
            )
        };

        struct object {
            std::vector<batch> batches;
            morph_targets morphTargets;
            std::vector<double> positions;
            std::vector<double> tex0;
            std::vector<unsigned> colors;
            std::vector<std::pair<float, unsigned>> influences;
            std::vector<double> normals;
            std::vector<unsigned> indices;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(batches),
                CXON_JSON_CLS_FIELD_ASIS(morphTargets),
                CXON_JSON_CLS_FIELD_ASIS(positions),
                CXON_JSON_CLS_FIELD_ASIS(tex0),
                CXON_JSON_CLS_FIELD_ASIS(colors),
                CXON_JSON_CLS_FIELD_ASIS(influences),
                CXON_JSON_CLS_FIELD_ASIS(normals),
                CXON_JSON_CLS_FIELD_ASIS(indices)
            )
        };

    }

    namespace numbers {

        using object = std::vector<double>;

    }

    namespace synth {
        
        using object_u8 = std::vector<unsigned char>;
        using object_s8 = std::vector<signed char>;
        using object_u16 = std::vector<std::uint16_t>;
        using object_s16 = std::vector<std::int16_t>;
        using object_u32 = std::vector<std::uint32_t>;
        using object_s32 = std::vector<std::int32_t>;
        using object_u64 = std::vector<std::uint64_t>;
        using object_s64 = std::vector<std::int64_t>;
        using object_fp32 = std::vector<float>;
        using object_fp64 = std::vector<double>;
        using object_null = std::vector<std::nullptr_t>;
        using object_bool = std::deque<bool>;
        using object_string = std::vector<std::string>;
        using object_array = std::vector<std::pair<double, double>>;
        struct point_2d {
            double x, y;
            CXON_JSON_CLS_MEMBER(point_2d,
                CXON_JSON_CLS_FIELD_ASIS(x),
                CXON_JSON_CLS_FIELD_ASIS(y)
            )
        };
        using object_object = std::vector<point_2d>;

    }

    namespace random {

        struct friend_ {
            unsigned id;
            std::string name;
            std::string phone;

            CXON_JSON_CLS_MEMBER(friend_,
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(phone)
            )
        };

        struct result {
            unsigned id;
            std::string avatar;
            unsigned age;
            bool admin;
            std::string name;
            std::string company;
            std::string phone;
            std::string email;
            std::string birthDate;
            std::vector<friend_> friends;
            std::string field;

            CXON_JSON_CLS_MEMBER(result,
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(avatar),
                CXON_JSON_CLS_FIELD_ASIS(age),
                CXON_JSON_CLS_FIELD_ASIS(admin),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(company),
                CXON_JSON_CLS_FIELD_ASIS(phone),
                CXON_JSON_CLS_FIELD_ASIS(email),
                CXON_JSON_CLS_FIELD_ASIS(birthDate),
                CXON_JSON_CLS_FIELD_ASIS(friends),
                CXON_JSON_CLS_FIELD_ASIS(field)
            )
        };

        struct object {
            unsigned id;
            std::string jsonrpc;
            unsigned total;
            std::vector<struct result> result;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(jsonrpc),
                CXON_JSON_CLS_FIELD_ASIS(total),
                CXON_JSON_CLS_FIELD_ASIS(result)
            )
        };

    }

#   ifdef CXON_HAS_LIB_STD_OPTIONAL
        namespace twitter {

            struct metadata {
                std::string result_type;
                std::string iso_language_code;

                CXON_JSON_CLS_MEMBER(metadata,
                    CXON_JSON_CLS_FIELD_ASIS(result_type),
                    CXON_JSON_CLS_FIELD_ASIS(iso_language_code)
                )
            };

            struct url {
                std::string url_;
                std::string expanded_url;
                std::string display_url;
                std::vector<unsigned> indices;

                CXON_JSON_CLS_MEMBER(url,
                    CXON_JSON_CLS_FIELD_NAME("url", url_),
                    CXON_JSON_CLS_FIELD_ASIS(expanded_url),
                    CXON_JSON_CLS_FIELD_ASIS(display_url),
                    CXON_JSON_CLS_FIELD_ASIS(indices)
                )
            };

            struct user_mention {
                std::string screen_name;
                std::string name;
                unsigned long long id;
                std::string id_str;
                std::vector<unsigned> indices;

                CXON_JSON_CLS_MEMBER(user_mention,
                    CXON_JSON_CLS_FIELD_ASIS(screen_name),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(id_str),
                    CXON_JSON_CLS_FIELD_ASIS(indices)
                )
            };

            struct size {
                unsigned w;
                unsigned h;
                std::string resize;

                CXON_JSON_CLS_MEMBER(size,
                    CXON_JSON_CLS_FIELD_ASIS(w),
                    CXON_JSON_CLS_FIELD_ASIS(h),
                    CXON_JSON_CLS_FIELD_ASIS(resize)
                )
            };

            struct sizes {
                size medium;
                size large;
                size thumb;
                size small;

                CXON_JSON_CLS_MEMBER(sizes,
                    CXON_JSON_CLS_FIELD_ASIS(medium),
                    CXON_JSON_CLS_FIELD_ASIS(large),
                    CXON_JSON_CLS_FIELD_ASIS(thumb),
                    CXON_JSON_CLS_FIELD_ASIS(small)
                )
            };

            struct media {
                unsigned long long id;
                std::string id_str;
                std::vector<unsigned> indices;
                std::string media_url;
                std::string media_url_https;
                std::string url;
                std::string display_url;
                std::string expanded_url;
                std::string type;
                struct sizes sizes;
                unsigned long long source_status_id;
                std::string source_status_id_str;

                CXON_JSON_CLS_MEMBER(media,
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(id_str),
                    CXON_JSON_CLS_FIELD_ASIS(indices),
                    CXON_JSON_CLS_FIELD_ASIS(media_url),
                    CXON_JSON_CLS_FIELD_ASIS(media_url_https),
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(display_url),
                    CXON_JSON_CLS_FIELD_ASIS(expanded_url),
                    CXON_JSON_CLS_FIELD_ASIS(type),
                    CXON_JSON_CLS_FIELD_ASIS(sizes),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(source_status_id      , self.source_status_id == 0),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(source_status_id_str  , self.source_status_id_str.empty())
                )
            };

            struct description {
                std::vector<url> urls;

                CXON_JSON_CLS_MEMBER(description,
                    CXON_JSON_CLS_FIELD_ASIS(urls)
                )
            };

            struct urls {
                std::vector<url> urls_;

                CXON_JSON_CLS_MEMBER(urls,
                    CXON_JSON_CLS_FIELD_NAME("urls", urls_)
                )
            };

            struct hashtag {
                std::string text;
                std::vector<unsigned> indices;

                CXON_JSON_CLS_MEMBER(hashtag,
                    CXON_JSON_CLS_FIELD_ASIS(text),
                    CXON_JSON_CLS_FIELD_ASIS(indices)
                )
            };

            struct entity {
                std::vector<hashtag> hashtags;
                std::vector<std::string> symbols;
                std::vector<struct url> urls;
                std::vector<user_mention> user_mentions;
                struct urls url;
                struct description description;
                std::vector<struct media> media;

                CXON_JSON_CLS_MEMBER(entity,
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(hashtags          , self.hashtags.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(symbols           , self.symbols.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(urls              , self.urls.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(user_mentions     , self.user_mentions.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(url               , self.url.urls_.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(description       , self.description.urls.empty()),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(media             , self.media.empty())
                )
            };

            struct user {
                unsigned long long id;
                std::string id_str;
                std::string name;
                std::string screen_name;
                std::string location;
                std::string description;
                std::optional<std::string> url;
                entity entities;
                bool protected_;
                unsigned followers_count;
                unsigned friends_count;
                unsigned listed_count;
                std::string created_at;
                unsigned favourites_count;
                std::optional<signed> utc_offset;
                std::optional<std::string> time_zone;
                bool geo_enabled;
                bool verified;
                unsigned statuses_count;
                std::string lang;
                bool contributors_enabled;
                bool is_translator;
                bool is_translation_enabled;
                std::string profile_background_color;
                std::string profile_background_image_url;
                std::string profile_background_image_url_https;
                bool profile_background_tile;
                std::string profile_image_url;
                std::string profile_image_url_https;
                std::string profile_banner_url;
                std::string profile_link_color;
                std::string profile_sidebar_border_color;
                std::string profile_sidebar_fill_color;
                std::string profile_text_color;
                bool profile_use_background_image;
                bool default_profile;
                bool default_profile_image;
                bool following;
                bool follow_request_sent;
                bool notifications;

                CXON_JSON_CLS_MEMBER(user,
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(id_str),
                    CXON_JSON_CLS_FIELD_ASIS(name),
                    CXON_JSON_CLS_FIELD_ASIS(screen_name),
                    CXON_JSON_CLS_FIELD_ASIS(location),
                    CXON_JSON_CLS_FIELD_ASIS(description),
                    CXON_JSON_CLS_FIELD_ASIS(url),
                    CXON_JSON_CLS_FIELD_ASIS(entities),
                    CXON_JSON_CLS_FIELD_NAME("protected", protected_),
                    CXON_JSON_CLS_FIELD_ASIS(followers_count),
                    CXON_JSON_CLS_FIELD_ASIS(friends_count),
                    CXON_JSON_CLS_FIELD_ASIS(listed_count),
                    CXON_JSON_CLS_FIELD_ASIS(created_at),
                    CXON_JSON_CLS_FIELD_ASIS(favourites_count),
                    CXON_JSON_CLS_FIELD_ASIS(utc_offset),
                    CXON_JSON_CLS_FIELD_ASIS(time_zone),
                    CXON_JSON_CLS_FIELD_ASIS(geo_enabled),
                    CXON_JSON_CLS_FIELD_ASIS(verified),
                    CXON_JSON_CLS_FIELD_ASIS(statuses_count),
                    CXON_JSON_CLS_FIELD_ASIS(lang),
                    CXON_JSON_CLS_FIELD_ASIS(contributors_enabled),
                    CXON_JSON_CLS_FIELD_ASIS(is_translator),
                    CXON_JSON_CLS_FIELD_ASIS(is_translation_enabled),
                    CXON_JSON_CLS_FIELD_ASIS(profile_background_color),
                    CXON_JSON_CLS_FIELD_ASIS(profile_background_image_url),
                    CXON_JSON_CLS_FIELD_ASIS(profile_background_image_url_https),
                    CXON_JSON_CLS_FIELD_ASIS(profile_background_tile),
                    CXON_JSON_CLS_FIELD_ASIS(profile_image_url),
                    CXON_JSON_CLS_FIELD_ASIS(profile_image_url_https),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(profile_banner_url, self.profile_banner_url.empty()),
                    CXON_JSON_CLS_FIELD_ASIS(profile_link_color),
                    CXON_JSON_CLS_FIELD_ASIS(profile_sidebar_border_color),
                    CXON_JSON_CLS_FIELD_ASIS(profile_sidebar_fill_color),
                    CXON_JSON_CLS_FIELD_ASIS(profile_text_color),
                    CXON_JSON_CLS_FIELD_ASIS(profile_use_background_image),
                    CXON_JSON_CLS_FIELD_ASIS(default_profile),
                    CXON_JSON_CLS_FIELD_ASIS(default_profile_image),
                    CXON_JSON_CLS_FIELD_ASIS(following),
                    CXON_JSON_CLS_FIELD_ASIS(follow_request_sent),
                    CXON_JSON_CLS_FIELD_ASIS(notifications)
                )
            };

            struct retweeted_status {
                struct metadata metadata;
                std::string created_at;
                unsigned long long id;
                std::string id_str;
                std::string text;
                std::string source;
                bool truncated;
                std::optional<unsigned long long> in_reply_to_status_id;
                std::optional<std::string> in_reply_to_status_id_str;
                std::optional<unsigned long long> in_reply_to_user_id;
                std::optional<std::string> in_reply_to_user_id_str;
                std::optional<std::string> in_reply_to_screen_name;
                struct user user;
                std::nullptr_t geo;
                std::nullptr_t coordinates;
                std::nullptr_t place;
                std::nullptr_t contributors;
                unsigned retweet_count;
                unsigned favorite_count;
                entity entities;
                bool favorited;
                bool retweeted;
                bool possibly_sensitive;
                std::string lang;

                CXON_JSON_CLS_MEMBER(retweeted_status,
                    CXON_JSON_CLS_FIELD_ASIS(metadata),
                    CXON_JSON_CLS_FIELD_ASIS(created_at),
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(id_str),
                    CXON_JSON_CLS_FIELD_ASIS(text),
                    CXON_JSON_CLS_FIELD_ASIS(source),
                    CXON_JSON_CLS_FIELD_ASIS(truncated),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_status_id),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_status_id_str),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_user_id),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_user_id_str),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_screen_name),
                    CXON_JSON_CLS_FIELD_ASIS(user),
                    CXON_JSON_CLS_FIELD_ASIS(geo),
                    CXON_JSON_CLS_FIELD_ASIS(coordinates),
                    CXON_JSON_CLS_FIELD_ASIS(place),
                    CXON_JSON_CLS_FIELD_ASIS(contributors),
                    CXON_JSON_CLS_FIELD_ASIS(retweet_count),
                    CXON_JSON_CLS_FIELD_ASIS(favorite_count),
                    CXON_JSON_CLS_FIELD_ASIS(entities),
                    CXON_JSON_CLS_FIELD_ASIS(favorited),
                    CXON_JSON_CLS_FIELD_ASIS(retweeted),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(possibly_sensitive, !self.possibly_sensitive),
                    CXON_JSON_CLS_FIELD_ASIS(lang)
                )
            };

            struct status {
                struct metadata metadata;
                std::string created_at;
                unsigned long long id;
                std::string id_str;
                std::string text;
                std::string source;
                bool truncated;
                std::optional<unsigned long long> in_reply_to_status_id;
                std::optional<std::string> in_reply_to_status_id_str;
                std::optional<unsigned long long> in_reply_to_user_id;
                std::optional<std::string> in_reply_to_user_id_str;
                std::optional<std::string> in_reply_to_screen_name;
                struct user user;
                std::nullptr_t geo;
                std::nullptr_t coordinates;
                std::nullptr_t place;
                std::nullptr_t contributors;
                struct retweeted_status retweeted_status; 
                unsigned retweet_count;
                unsigned favorite_count;
                entity entities;
                bool favorited;
                bool retweeted;
                bool possibly_sensitive;
                std::string lang;

                CXON_JSON_CLS_MEMBER(status,
                    CXON_JSON_CLS_FIELD_ASIS(metadata),
                    CXON_JSON_CLS_FIELD_ASIS(created_at),
                    CXON_JSON_CLS_FIELD_ASIS(id),
                    CXON_JSON_CLS_FIELD_ASIS(id_str),
                    CXON_JSON_CLS_FIELD_ASIS(text),
                    CXON_JSON_CLS_FIELD_ASIS(source),
                    CXON_JSON_CLS_FIELD_ASIS(truncated),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_status_id),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_status_id_str),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_user_id),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_user_id_str),
                    CXON_JSON_CLS_FIELD_ASIS(in_reply_to_screen_name),
                    CXON_JSON_CLS_FIELD_ASIS(user),
                    CXON_JSON_CLS_FIELD_ASIS(geo),
                    CXON_JSON_CLS_FIELD_ASIS(coordinates),
                    CXON_JSON_CLS_FIELD_ASIS(place),
                    CXON_JSON_CLS_FIELD_ASIS(contributors),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(retweeted_status      , self.retweeted_status.created_at.empty()),
                    CXON_JSON_CLS_FIELD_ASIS(retweet_count),
                    CXON_JSON_CLS_FIELD_ASIS(favorite_count),
                    CXON_JSON_CLS_FIELD_ASIS(entities),
                    CXON_JSON_CLS_FIELD_ASIS(favorited),
                    CXON_JSON_CLS_FIELD_ASIS(retweeted),
                    CXON_JSON_CLS_FIELD_ASIS_DFLT(possibly_sensitive    , !self.possibly_sensitive),
                    CXON_JSON_CLS_FIELD_ASIS(lang)
                )
            };
    
            struct search_metadata {
                float completed_in;
                unsigned long long max_id;
                std::string max_id_str;
                std::string next_results;
                std::string query;
                std::string refresh_url;
                unsigned count;
                unsigned since_id;
                std::string since_id_str;

                CXON_JSON_CLS_MEMBER(search_metadata,
                    CXON_JSON_CLS_FIELD_ASIS(completed_in),
                    CXON_JSON_CLS_FIELD_ASIS(max_id),
                    CXON_JSON_CLS_FIELD_ASIS(max_id_str),
                    CXON_JSON_CLS_FIELD_ASIS(next_results),
                    CXON_JSON_CLS_FIELD_ASIS(query),
                    CXON_JSON_CLS_FIELD_ASIS(refresh_url),
                    CXON_JSON_CLS_FIELD_ASIS(count),
                    CXON_JSON_CLS_FIELD_ASIS(since_id),
                    CXON_JSON_CLS_FIELD_ASIS(since_id_str)
                )
            };

            struct object {
                std::vector<status> statuses;
                struct search_metadata search_metadata;

                CXON_JSON_CLS_MEMBER(object,
                    CXON_JSON_CLS_FIELD_ASIS(statuses),
                    CXON_JSON_CLS_FIELD_ASIS(search_metadata)
                )
            };

        }
#   endif

    namespace update_center {

        struct core {
            std::string buildDate;
            std::string name;
            std::string sha1;
            std::string url;
            std::string version;

            CXON_JSON_CLS_MEMBER(core,
                CXON_JSON_CLS_FIELD_ASIS(buildDate),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(sha1),
                CXON_JSON_CLS_FIELD_ASIS(url),
                CXON_JSON_CLS_FIELD_ASIS(version)
            )
        };

        struct dependency {
            std::string name;
            bool optional;
            std::string version;

            CXON_JSON_CLS_MEMBER(dependency,
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(optional),
                CXON_JSON_CLS_FIELD_ASIS(version)
            )
        };

        struct developer {
            std::string developerId;
            std::string email;
            std::string name;

            CXON_JSON_CLS_MEMBER(developer,
                CXON_JSON_CLS_FIELD_ASIS(developerId),
                CXON_JSON_CLS_FIELD_ASIS(email),
                CXON_JSON_CLS_FIELD_ASIS(name)
            )
        };

        struct plugin {
            std::string buildDate;
            std::string compatibleSinceVersion;
            std::vector<dependency> dependencies;
            std::vector<developer> developers;
            std::string excerpt;
            std::string gav;
            std::vector<std::string> labels;
            std::string name;
            std::string previousTimestamp;
            std::string previousVersion;
            std::string releaseTimestamp;
            std::string requiredCore;
            std::string scm;
            std::string sha1;
            std::string title;
            std::string url;
            std::string version;
            std::string wiki;

            CXON_JSON_CLS_MEMBER(plugin,
                CXON_JSON_CLS_FIELD_ASIS(buildDate),
                CXON_JSON_CLS_FIELD_ASIS(compatibleSinceVersion),
                CXON_JSON_CLS_FIELD_ASIS(dependencies),
                CXON_JSON_CLS_FIELD_ASIS(developers),
                CXON_JSON_CLS_FIELD_ASIS(excerpt),
                CXON_JSON_CLS_FIELD_ASIS(gav),
                CXON_JSON_CLS_FIELD_ASIS(labels),
                CXON_JSON_CLS_FIELD_ASIS(name),
                CXON_JSON_CLS_FIELD_ASIS(previousTimestamp),
                CXON_JSON_CLS_FIELD_ASIS(previousVersion),
                CXON_JSON_CLS_FIELD_ASIS(releaseTimestamp),
                CXON_JSON_CLS_FIELD_ASIS(requiredCore),
                CXON_JSON_CLS_FIELD_ASIS(scm),
                CXON_JSON_CLS_FIELD_ASIS(sha1),
                CXON_JSON_CLS_FIELD_ASIS(title),
                CXON_JSON_CLS_FIELD_ASIS(url),
                CXON_JSON_CLS_FIELD_ASIS(version),
                CXON_JSON_CLS_FIELD_ASIS(wiki)
            )
        };

        struct signature_ {
            std::vector<std::string> certificates;
            std::string correct_digest;
            std::string correct_signature;
            std::string digest;
            std::string signature;

            CXON_JSON_CLS_MEMBER(signature_,
                CXON_JSON_CLS_FIELD_ASIS(certificates),
                CXON_JSON_CLS_FIELD_ASIS(correct_digest),
                CXON_JSON_CLS_FIELD_ASIS(correct_signature),
                CXON_JSON_CLS_FIELD_ASIS(digest),
                CXON_JSON_CLS_FIELD_ASIS(signature)
            )
        };

        struct object {
            std::string connectionCheckUrl;
            struct core core;
            std::string id;
            cxon::ordered::object<std::string, plugin> plugins;
            signature_ signature;
            std::string updateCenterVersion;

            CXON_JSON_CLS_MEMBER(object,
                CXON_JSON_CLS_FIELD_ASIS(connectionCheckUrl),
                CXON_JSON_CLS_FIELD_ASIS(core),
                CXON_JSON_CLS_FIELD_ASIS(id),
                CXON_JSON_CLS_FIELD_ASIS(plugins),
                CXON_JSON_CLS_FIELD_ASIS(signature),
                CXON_JSON_CLS_FIELD_ASIS(updateCenterVersion)
            )
        };

    }

}

#endif // CXON_JSON_TEST_NATIVE_HXX_
